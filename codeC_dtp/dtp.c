// Nhan ma tran CSR voi vector
#include <stdio.h>              
#include <stdlib.h>
#include <stdbool.h>

typedef struct Sparse_CSR {
    size_t n_rows;
    size_t n_cols;
    size_t n_nz;
    size_t* row_ptrs;
    size_t* col_indices;
    double* values;
} Sparse_CSR;

// Ham doc ma tran CSR tu file
int read_csr_matrix_from_file(const char* filename, Sparse_CSR* A_csr);

// Ham doc vector tu file
int read_vector_from_file(const char* filename, double** vector, size_t expected_size);

// Ham nhan ma tran CSR voi vector
int matrix_vector_sparse_csr(
    const Sparse_CSR* A_csr,
    const double* vec,
    double* res
);

// Ham ghi ket qua ra file
int write_result_to_file(const char* filename, const double* result, size_t size);

// Ham giai phong bo nho
int free_sparse_csr(Sparse_CSR* A_csr);

// Ham in ma tran CSR (de kiem tra)
int print_sparse_csr(const Sparse_CSR* A_csr);

int main(int argc, char** argv) {
    const char* matrix_file = "output_csr.txt";    // input
    const char* vector_file = "vector.txt";         // vector
    const char* output_file = "output_dtp.txt";     // output 
    
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
    double* vector = NULL;
    
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
        printf("vector[%zu] = %.2f\n", i, vector[i]);
    }
    
    // Cap phat bo nho cho ket qua
    double* result = malloc(A_csr.n_rows * sizeof(double));
    if (!result) {
        printf("Loi: Khong the cap phat bo nho cho ket qua!\n");
        free_sparse_csr(&A_csr);
        free(vector);
        return EXIT_FAILURE;
    }
    
    // Thuc hien phep nhan ma tran - vector
    printf("\nDang thuc hien phep nhan ma tran CSR voi vector...\n");
    if (matrix_vector_sparse_csr(&A_csr, vector, result) != EXIT_SUCCESS) {
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
        printf("result[%zu] = %.6f\n", i, result[i]);
    }
    
    // Giai phong bo nho
    free_sparse_csr(&A_csr);
    free(vector);
    free(result);
    
    return EXIT_SUCCESS;
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
    A_csr->values = malloc(A_csr->n_nz * sizeof(double));
    
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
        if (fscanf(file, "%lf", &A_csr->values[i]) != 1) {
            printf("Loi: Khong the doc values[%zu]!\n", i);
            fclose(file);
            return EXIT_FAILURE;
        }
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

int read_vector_from_file(const char* filename, double** vector, size_t expected_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file vector %s\n", filename);
        return EXIT_FAILURE;
    }
    
    // Cap phat bo nho cho vector
    *vector = malloc(expected_size * sizeof(double));
    if (!*vector) {
        printf("Loi: Khong the cap phat bo nho cho vector!\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    
    // Doc cac phan tu cua vector
    for (size_t i = 0; i < expected_size; ++i) {
        if (fscanf(file, "%lf", &(*vector)[i]) != 1) {
            printf("Loi: Khong the doc vector[%zu]!\n", i);
            printf("Kiem tra lai kich thuoc vector trong file!\n");
            fclose(file);
            free(*vector);
            return EXIT_FAILURE;
        }
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}

int matrix_vector_sparse_csr(
    const Sparse_CSR* A_csr,
    const double* vec,
    double* res
) {
    for (size_t i = 0; i < A_csr->n_rows; ++i) {
        res[i] = 0.0;
        size_t nz_start = A_csr->row_ptrs[i];
        size_t nz_end = A_csr->row_ptrs[i+1];
        for (size_t nz_id = nz_start; nz_id < nz_end; ++nz_id) {
            size_t j = A_csr->col_indices[nz_id];
            double val = A_csr->values[nz_id];
            res[i] += val * vec[j];
        }
    }
    return EXIT_SUCCESS;
}

int write_result_to_file(const char* filename, const double* result, size_t size) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Loi: Khong the tao file %s\n", filename);
        return EXIT_FAILURE;
    }
    
    fprintf(file, "Ket qua phep nhan ma tran CSR voi vector:\n");
    fprintf(file, "Kich thuoc ket qua: %zu\n", size);
    fprintf(file, "--------------------------------------\n");
    
    for (size_t i = 0; i < size; ++i) {
        fprintf(file, "result[%zu] = %.6f\n", i, result[i]);
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