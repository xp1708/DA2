#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Sparse_CSR {
    size_t n_rows;
    size_t n_cols;
    size_t n_nz;
    size_t* row_ptrs;
    size_t* col_indices;
    double* values;
} Sparse_CSR;

// Doc ma tran thuong tu file
int read_dense_matrix(const char* filename, double** matrix, size_t* n_rows, size_t* n_cols);

// Chuyen doi ma tran thuong thanh CSR
int convert_dense_to_csr(const double* matrix, size_t n_rows, size_t n_cols, Sparse_CSR* A_csr);

// Ghi ma tran CSR ra file
int write_csr_to_file(const char* filename, const Sparse_CSR* A_csr);

// In ma tran CSR
int print_sparse_csr(const Sparse_CSR* A_csr);

// Giai phong bo nho
int free_sparse_csr(Sparse_CSR* A_csr);

int main(int argc, char** argv) {
    // If no command line arguments are provided, use default filenames
    const char* input_filename = "input_dense.txt";
    const char* output_filename = "output_csr.txt";

    // If command line arguments are provided, use them
    if (argc == 3) {
        input_filename = argv[1];
        output_filename = argv[2];
    } else if (argc != 1) { // If arguments are provided but not 2
        printf("Cach su dung: %s [file_ma_tran_thuong] [file_ma_tran_csr]\n", argv[0]);
        return EXIT_FAILURE;
    }

    double* matrix = NULL;
    size_t n_rows, n_cols;
    Sparse_CSR A_csr;

    // Doc ma tran thuong tu file
    printf("Dang doc ma tran tu file: %s\n", input_filename);
    if (read_dense_matrix(input_filename, &matrix, &n_rows, &n_cols) != EXIT_SUCCESS) {
        printf("Loi: Khong the doc ma tran tu file!\n");
        return EXIT_FAILURE;
    }

    printf("Ma tran da doc: %zu x %zu\n", n_rows, n_cols);

    // Chuyen doi thanh CSR
    printf("Dang chuyen doi thanh dinh dang CSR...\n");
    if (convert_dense_to_csr(matrix, n_rows, n_cols, &A_csr) != EXIT_SUCCESS) {
        printf("Loi: Khong the chuyen doi ma tran!\n");
        free(matrix);
        return EXIT_FAILURE;
    }

    printf("Chuyen doi thanh cong! So phan tu khac 0: %zu\n", A_csr.n_nz);

    // In ma tran CSR
    printf("\nMa tran CSR:\n");
    print_sparse_csr(&A_csr);

    // Ghi ma tran CSR ra file
    printf("\nDang ghi ma tran CSR ra file: %s\n", output_filename);
    if (write_csr_to_file(output_filename, &A_csr) != EXIT_SUCCESS) {
        printf("Loi: Khong the ghi file!\n");
        free(matrix);
        free_sparse_csr(&A_csr);
        return EXIT_FAILURE;
    }

    printf("Ghi file thanh cong!\n");

    // Giai phong bo nho
    free(matrix);
    free_sparse_csr(&A_csr);

    return EXIT_SUCCESS;
}

int read_dense_matrix(const char* filename, double** matrix, size_t* n_rows, size_t* n_cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file %s\n", filename);
        return EXIT_FAILURE;
    }

    // Doc kich thuoc ma tran
    if (fscanf(file, "%zu %zu", n_rows, n_cols) != 2) {
        printf("Loi: Khong the doc kich thuoc ma tran\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Cap phat bo nho
    *matrix = malloc((*n_rows) * (*n_cols) * sizeof(double));
    if (!*matrix) {
        printf("Loi: Khong the cap phat bo nho\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Doc cac phan tu ma tran
    for (size_t i = 0; i < (*n_rows) * (*n_cols); ++i) {
        if (fscanf(file, "%lf", &(*matrix)[i]) != 1) {
            printf("Loi: Khong the doc phan tu ma tran\n");
            free(*matrix);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int convert_dense_to_csr(const double* matrix, size_t n_rows, size_t n_cols, Sparse_CSR* A_csr) {
    // Dem so phan tu khac 0
    size_t n_nz = 0;
    for (size_t i = 0; i < n_rows * n_cols; ++i) {
        if (matrix[i] != 0.0) {
            n_nz++;
        }
    }

    // Khoi tao struct CSR
    A_csr->n_rows = n_rows;
    A_csr->n_cols = n_cols;
    A_csr->n_nz = n_nz;
    A_csr->row_ptrs = malloc((n_rows + 1) * sizeof(size_t));
    A_csr->col_indices = malloc(n_nz * sizeof(size_t));
    A_csr->values = malloc(n_nz * sizeof(double));

    if (!A_csr->row_ptrs || !A_csr->col_indices || !A_csr->values) {
        printf("Loi: Khong the cap phat bo nho cho CSR\n");
        return EXIT_FAILURE;
    }

    // Chuyen doi
    size_t nz_id = 0;
    for (size_t i = 0; i < n_rows; ++i) {
        A_csr->row_ptrs[i] = nz_id;
        for (size_t j = 0; j < n_cols; ++j) {
            double val = matrix[i * n_cols + j];
            if (fabs(val) > 1e-9) {  // dùng ngưỡng nhỏ để bỏ qua các giá trị gần 0
                A_csr->col_indices[nz_id] = j;
                A_csr->values[nz_id] = val;
                nz_id++;
            }
        }
    }
    A_csr->row_ptrs[n_rows] = nz_id;

    return EXIT_SUCCESS;
}

int write_csr_to_file(const char* filename, const Sparse_CSR* A_csr) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Loi: Khong the tao file %s\n", filename);
        return EXIT_FAILURE;
    }

    // Ghi thong tin ma tran
    fprintf(file, "%zu %zu %zu\n", A_csr->n_rows, A_csr->n_cols, A_csr->n_nz);

    // Ghi row_ptrs
    for (size_t i = 0; i <= A_csr->n_rows; ++i) {
        fprintf(file, "%zu ", A_csr->row_ptrs[i]);
    }
    fprintf(file, "\n");

    // Ghi col_indices
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        fprintf(file, "%zu ", A_csr->col_indices[i]);
    }
    fprintf(file, "\n");

    // Ghi values
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        fprintf(file, "%g ", A_csr->values[i]);
    }
    fprintf(file, "\n");

    fclose(file);
    return EXIT_SUCCESS;
}

int print_sparse_csr(const Sparse_CSR* A_csr) {
    printf("Kich thuoc: %zu x %zu, So phan tu khac 0: %zu\n",
           A_csr->n_rows, A_csr->n_cols, A_csr->n_nz);
    printf("row\tcol\tval\n");
    printf("----------------\n");

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