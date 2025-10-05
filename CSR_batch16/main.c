// Nhan ma tran CSR voi vector - Phien ban datapath batch 16 phan tu
#include <stdio.h>              
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define BATCH_SIZE 16
#define WINDOW_SIZE 16

typedef struct Sparse_CSR {
    size_t n_rows;
    size_t n_cols;
    size_t n_nz;
    size_t* row_ptrs;
    size_t* col_indices;
    int64_t* values;
} Sparse_CSR;
    
// Struct mo phong datapath registers
typedef struct DatapathRegisters {
    size_t current_row_start;
    size_t current_row_end;
    int64_t values_batch[BATCH_SIZE];
    size_t col_indices_batch[BATCH_SIZE];
    int64_t vector_window[WINDOW_SIZE];
    size_t window_start_idx;
    int64_t accumulator;
} DatapathRegisters;

// Ham doc ma tran CSR tu file
int read_csr_matrix_from_file(const char* filename, Sparse_CSR* A_csr);

// Ham doc vector tu file
int read_vector_from_file(const char* filename, int64_t** vector, size_t expected_size);

// Ham nhan ma tran CSR voi vector theo datapath design
int matrix_vector_sparse_csr_batched(
    const Sparse_CSR* A_csr,
    const int64_t* vec,
    int64_t* res
);

// Ham load vector window theo cong thuc (col_index/window_size)*window_size
void load_vector_window(const int64_t* vec, size_t col_index, 
                       int64_t* window, size_t* window_start, size_t vec_size);

// Ham kiem tra xem co can load window moi khong
bool need_new_window(size_t col_index, size_t current_window_start);

// Ham load batch du lieu ma tran (values + col_indices)
size_t load_matrix_batch(const Sparse_CSR* A_csr, size_t start_idx, size_t end_idx,
                        int64_t* values_batch, size_t* col_indices_batch);

// Ham ghi ket qua ra file
int write_result_to_file(const char* filename, const int64_t* result, size_t size);

// Ham giai phong bo nho
int free_sparse_csr(Sparse_CSR* A_csr);

// Ham in ma tran CSR (de kiem tra)
int print_sparse_csr(const Sparse_CSR* A_csr);

int main(int argc, char** argv) {
    const char* matrix_file = "E:/DA1/codeC_dtp/output_convert_to_CSR.txt";    // input
    const char* vector_file = "E:/DA1/codeC_dtp/input_vector.txt";      // vector
    const char* output_file = "E:/DA2/csr_batch16/output_dtp.txt";     // output 
    
    // Neu nguoi dung cung cap ten file
    if (argc >= 2) {
        matrix_file = argv[1];
    }
    if (argc >= 3) {
        vector_file = argv[2];
    }
    if (argc >= 4) {
        output_file = argv[3];
    }
    
    printf("Dang doc ma tran CSR tu file: %s\n", matrix_file);
    
    Sparse_CSR A_csr;
    int64_t* vector = NULL;
    
    // Doc ma tran CSR tu file
    if (read_csr_matrix_from_file(matrix_file, &A_csr) != EXIT_SUCCESS) {
        printf("Loi: Khong the doc ma tran tu file!\n");
        return EXIT_FAILURE;
    }
    
    printf("Doc ma tran thanh cong!\n");
    printf("Kich thuoc ma tran: %zu x %zu\n", A_csr.n_rows, A_csr.n_cols);
    printf("So phan tu khac khong: %zu\n", A_csr.n_nz);
    
    printf("\nDang doc vector tu file: %s\n", vector_file);
    
    // Doc vector tu file
    if (read_vector_from_file(vector_file, &vector, A_csr.n_cols) != EXIT_SUCCESS) {
        printf("Loi: Khong the doc vector tu file!\n");
        free_sparse_csr(&A_csr);
        return EXIT_FAILURE;
    }
    
    printf("Doc vector thanh cong!\n");
    
    // Hien thi ma tran CSR (tuy chon)
    printf("\nThong tin ma tran CSR:\n");
    print_sparse_csr(&A_csr);
    
    // Hien thi vector
    printf("\nVector da nhap:\n");
    for (size_t i = 0; i < A_csr.n_cols; ++i) {
        printf("vector[%zu] = %" PRId64 "\n", i, vector[i]);
    }
    
    // Cap phat bo nho cho ket qua
    int64_t* result = malloc(A_csr.n_rows * sizeof(int64_t));
    if (!result) {
        printf("Loi: Khong the cap phat bo nho cho ket qua!\n");
        free_sparse_csr(&A_csr);
        free(vector);
        return EXIT_FAILURE;
    }
    
    // Thuc hien phep nhan ma tran - vector theo datapath design
    printf("\nDang thuc hien phep nhan ma tran CSR voi vector (datapath batched)...\n");
    if (matrix_vector_sparse_csr_batched(&A_csr, vector, result) != EXIT_SUCCESS) {
        printf("Loi: Khong the thuc hien phep nhan!\n");
        free_sparse_csr(&A_csr);
        free(vector);
        free(result);
        return EXIT_FAILURE;
    }
    
    // Ghi ket qua ra file
    printf("Dang ghi ket qua ra file: %s\n", output_file);
    if (write_result_to_file(output_file, result, A_csr.n_rows) != EXIT_SUCCESS) {
        printf("Loi: Khong the ghi ket qua ra file!\n");
        free_sparse_csr(&A_csr);
        free(vector);
        free(result);
        return EXIT_FAILURE;
    }
    
    printf("Hoan thanh! Ket qua da duoc ghi vao file %s\n", output_file);
    
    // Hien thi ket qua tren man hinh
    printf("\nKet qua phep nhan A * x:\n");
    for (size_t i = 0; i < A_csr.n_rows; ++i) {
        printf("result[%zu] = %" PRId64 "\n", i, result[i]);
    }
    
    // Giai phong bo nho
    free_sparse_csr(&A_csr);
    free(vector);
    free(result);
    
    return EXIT_SUCCESS;
}

int matrix_vector_sparse_csr_batched(
    const Sparse_CSR* A_csr,
    const int64_t* vec,
    int64_t* res
) {
    DatapathRegisters datapath;
    
    printf("\n=== BAT DAU XU LY THEO DATAPATH DESIGN ===\n");
    
    // Xu ly tung hang
    for (size_t row = 0; row < A_csr->n_rows; ++row) {
        // Buoc 1: Cap nhat row pointers cho hang hien tai
        datapath.current_row_start = A_csr->row_ptrs[row];
        datapath.current_row_end = A_csr->row_ptrs[row + 1];
        datapath.accumulator = 0;
        datapath.window_start_idx = SIZE_MAX; // Danh dau chua load window nao
        
        size_t row_nnz = datapath.current_row_end - datapath.current_row_start;
        
        printf("\nXu ly hang %zu: %zu phan tu khac 0 (tu vi tri %zu den %zu)\n", 
               row, row_nnz, datapath.current_row_start, datapath.current_row_end - 1);
        
        if (row_nnz == 0) {
            res[row] = 0.0;
            continue;
        }
        
        // Xu ly theo batch
        size_t processed_elements = 0;
        size_t current_batch_start = datapath.current_row_start;
        
        while (processed_elements < row_nnz) {
            // Buoc 2: Load batch du lieu ma tran (toi da 16 cap gia tri)
            size_t batch_end = current_batch_start + BATCH_SIZE;
            if (batch_end > datapath.current_row_end) {
                batch_end = datapath.current_row_end;
            }
            
            size_t actual_batch_size = load_matrix_batch(A_csr, current_batch_start, 
                                                       batch_end, datapath.values_batch, 
                                                       datapath.col_indices_batch);
            
            printf("  Batch %zu: Load %zu phan tu (tu %zu den %zu)\n", 
                   processed_elements / BATCH_SIZE, actual_batch_size, 
                   current_batch_start, batch_end - 1);
            
            // Buoc 3: Xu ly tung phan tu trong batch
            for (size_t b = 0; b < actual_batch_size; ++b) {
                size_t col_idx = datapath.col_indices_batch[b];
                int64_t matrix_val = datapath.values_batch[b];
                
                // Kiem tra co can load window moi khong
                if (need_new_window(col_idx, datapath.window_start_idx)) {
                    load_vector_window(vec, col_idx, datapath.vector_window, 
                                     &datapath.window_start_idx, A_csr->n_cols);
                    printf("    Load vector window moi: bat dau tu vi tri %zu\n", 
                           datapath.window_start_idx);
                }
                
                // Tinh toan: matrix_val * vector[col_idx]
                size_t window_offset = col_idx - datapath.window_start_idx;
                int64_t vector_val = datapath.vector_window[window_offset];
                int64_t product = matrix_val * vector_val;
                
                // Cong don ket qua (tan dung tinh giao hoan)
                datapath.accumulator += product;
                
                printf("    Tinh toan: %" PRId64 " * %" PRId64 " = %" PRId64 " (cong don = %" PRId64 ")\n",
                       matrix_val, vector_val, product, datapath.accumulator);
            }
            
            processed_elements += actual_batch_size;
            current_batch_start = batch_end;
        }
        
        // Luu ket qua cho hang
        res[row] = datapath.accumulator;
        printf("  Ket qua hang %zu: %" PRId64 "\n", row, res[row]);
    }
    
    printf("\n=== HOAN THANH XU LY DATAPATH ===\n");
    return EXIT_SUCCESS;
}

void load_vector_window(const int64_t* vec, size_t col_index, 
                       int64_t* window, size_t* window_start, size_t vec_size) {
    // Su dung cong thuc: (col_index / WINDOW_SIZE) * WINDOW_SIZE
    *window_start = (col_index / WINDOW_SIZE) * WINDOW_SIZE;
    
    // Load toi da WINDOW_SIZE phan tu, nhung khong vuot qua kich thuoc vector
    size_t elements_to_load = WINDOW_SIZE;
    if (*window_start + WINDOW_SIZE > vec_size) {
        elements_to_load = vec_size - *window_start;
    }
    
    // Copy du lieu vao window
    for (size_t i = 0; i < elements_to_load; ++i) {
        window[i] = vec[*window_start + i];
    }
    
    // Neu khong du WINDOW_SIZE phan tu, dien 0 vao cac vi tri con lai
    for (size_t i = elements_to_load; i < WINDOW_SIZE; ++i) {
        window[i] = 0;
    }
}

bool need_new_window(size_t col_index, size_t current_window_start) {
    // Chua load window nao
    if (current_window_start == SIZE_MAX) {
        return true;
    }
    
    // Kiem tra xem col_index co nam ngoai window hien tai khong
    return (col_index < current_window_start || 
            col_index >= current_window_start + WINDOW_SIZE);
}

size_t load_matrix_batch(const Sparse_CSR* A_csr, size_t start_idx, size_t end_idx,
                        int64_t* values_batch, size_t* col_indices_batch) {
    size_t batch_size = end_idx - start_idx;
    
    // Copy du lieu vao batch (khong load du thua)
    for (size_t i = 0; i < batch_size; ++i) {
        values_batch[i] = A_csr->values[start_idx + i];
        col_indices_batch[i] = A_csr->col_indices[start_idx + i];
    }
    
    return batch_size;
}

int read_csr_matrix_from_file(const char* filename, Sparse_CSR* A_csr) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file ma tran %s\n", filename);
        return EXIT_FAILURE;
    }
    
    // Doc kich thuoc ma tran va so phan tu khac khong
    if (fscanf(file, "%zu %zu %zu", &A_csr->n_rows, &A_csr->n_cols, &A_csr->n_nz) != 3) {
        printf("Loi: Khong the doc kich thuoc ma tran!\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Cap phat bo nho cho cac mang
    A_csr->row_ptrs = malloc((A_csr->n_rows + 1) * sizeof(size_t));
    A_csr->col_indices = malloc(A_csr->n_nz * sizeof(size_t));
    A_csr->values = malloc(A_csr->n_nz * sizeof(int64_t));
    
    if (!A_csr->row_ptrs || !A_csr->col_indices || !A_csr->values) {
        printf("Loi: Khong the cap phat bo nho cho ma tran!\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Doc mang row_ptrs
    for (size_t i = 0; i <= A_csr->n_rows; ++i) {
        if (fscanf(file, "%zu", &A_csr->row_ptrs[i]) != 1) {
            printf("Loi: Khong the doc row_ptrs[%zu]!\n", i);
            fclose(file);
            return EXIT_FAILURE;
        }
    }
    
    // Doc mang col_indices
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        if (fscanf(file, "%zu", &A_csr->col_indices[i]) != 1) {
            printf("Loi: Khong the doc col_indices[%zu]!\n", i);
            fclose(file);
            return EXIT_FAILURE;
        }
    }
    
    // Doc mang values
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        long long temp_val;
        if (fscanf(file, "%lld", &temp_val) != 1) {
            printf("Loi: Khong the doc values[%zu]!\n", i);
            fclose(file);
            return EXIT_FAILURE;
        }
        A_csr->values[i] = (int64_t)temp_val;
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

int read_vector_from_file(const char* filename, int64_t** vector, size_t expected_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file vector %s\n", filename);
        return EXIT_FAILURE;
    }
    
    // Cap phat bo nho cho vector
    *vector = malloc(expected_size * sizeof(int64_t));
    if (!*vector) {
        printf("Loi: Khong the cap phat bo nho cho vector!\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Doc cac phan tu cua vector
    for (size_t i = 0; i < expected_size; ++i) {
        unsigned int hex_value;
        if (fscanf(file, "%x", &hex_value) != 1) {
            printf("Loi: Khong the doc phan tu vector[%zu]!\n", i);
            printf("Phan tu phai la so hex (vi du: 00000001, 0000000A)\n");
            free(*vector);
            fclose(file);
            return EXIT_FAILURE;
        }
        (*vector)[i] = (int64_t)hex_value;
        printf("Doc thanh cong vector[%zu] = 0x%X = %" PRId64 "\n", i, hex_value, (*vector)[i]);
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

int write_result_to_file(const char* filename, const int64_t* result, size_t size) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Loi: Khong the tao file %s\n", filename);
        return EXIT_FAILURE;
    }
    
    fprintf(file, "Ket qua phep nhan ma tran CSR voi vector (Datapath Batched):\n");
    fprintf(file, "Kich thuoc ket qua: %zu\n", size);
    fprintf(file, "--------------------------------------\n");
    
    for (size_t i = 0; i < size; ++i) {
        fprintf(file, "result[%zu] = %" PRId64 "\n", i, result[i]);
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

int print_sparse_csr(const Sparse_CSR* A_csr) {
    printf("Hang\tCot\tGia tri\n");
    printf("-------------------\n");
    for (size_t i = 0; i < A_csr->n_rows; ++i) {
        size_t nz_start = A_csr->row_ptrs[i];
        size_t nz_end = A_csr->row_ptrs[i+1];
        for (size_t nz_id = nz_start; nz_id < nz_end; ++nz_id) {
            size_t j = A_csr->col_indices[nz_id];
            double val = A_csr->values[nz_id];
            printf("%zu\t%zu\t%.2f\n", i, j, val);
        }
    }
    return EXIT_SUCCESS;
}

int free_sparse_csr(Sparse_CSR* A_csr) {
    if (A_csr->row_ptrs) free(A_csr->row_ptrs);
    if (A_csr->col_indices) free(A_csr->col_indices);
    if (A_csr->values) free(A_csr->values);
    
    A_csr->row_ptrs = NULL;
    A_csr->col_indices = NULL;
    A_csr->values = NULL;
    
    return EXIT_SUCCESS;
}