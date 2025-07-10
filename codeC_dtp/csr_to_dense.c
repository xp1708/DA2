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

// Doc ma tran CSR tu file
int read_csr_from_file(const char* filename, Sparse_CSR* A_csr);

// Chuyen doi CSR thanh ma tran thuong
int convert_csr_to_dense(const Sparse_CSR* A_csr, double** matrix);

// Ghi ma tran thuong ra file
int write_dense_matrix(const char* filename, const double* matrix, size_t n_rows, size_t n_cols);

// In ma tran thuong
int print_dense_matrix(const double* matrix, size_t n_rows, size_t n_cols);

// In ma tran CSR
int print_sparse_csr(const Sparse_CSR* A_csr);

// Giai phong bo nho
int free_sparse_csr(Sparse_CSR* A_csr);

int main() {
    const char* input_filename = "csr_to_dense.txt";
    const char* output_filename = "output_csr.txt";

    Sparse_CSR A_csr;
    double* matrix = NULL;

    printf("Dang doc ma tran CSR tu file: %s\n", input_filename);
    if (read_csr_from_file(input_filename, &A_csr) != EXIT_SUCCESS) {
        printf("Loi: Khong the doc ma tran CSR tu file!\n");
        return EXIT_FAILURE;
    }

    printf("Ma tran CSR da doc: %zu x %zu, %zu phan tu khac 0\n",
           A_csr.n_rows, A_csr.n_cols, A_csr.n_nz);

    printf("\nMa tran CSR:\n");
    print_sparse_csr(&A_csr);

    printf("\nDang chuyen doi thanh ma tran thuong...\n");
    if (convert_csr_to_dense(&A_csr, &matrix) != EXIT_SUCCESS) {
        printf("Loi: Khong the chuyen doi ma tran!\n");
        free_sparse_csr(&A_csr);
        return EXIT_FAILURE;
    }

    printf("Chuyen doi thanh cong!\n");

    printf("\nMa tran thuong:\n");
    print_dense_matrix(matrix, A_csr.n_rows, A_csr.n_cols);

    printf("\nDang ghi ma tran thuong ra file: %s\n", output_filename);
    if (write_dense_matrix(output_filename, matrix, A_csr.n_rows, A_csr.n_cols) != EXIT_SUCCESS) {
        printf("Loi: Khong the ghi file!\n");
        free(matrix);
        free_sparse_csr(&A_csr);
        return EXIT_FAILURE;
    }

    printf("Ghi file thanh cong!\n");

    free(matrix);
    free_sparse_csr(&A_csr);

    return EXIT_SUCCESS;
}


int read_csr_from_file(const char* filename, Sparse_CSR* A_csr) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file %s\n", filename);
        return EXIT_FAILURE;
    }

    // Doc thong tin ma tran
    if (fscanf(file, "%zu %zu %zu", &A_csr->n_rows, &A_csr->n_cols, &A_csr->n_nz) != 3) {
        printf("Loi: Khong the doc thong tin ma tran\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Cap phat bo nho
    A_csr->row_ptrs = malloc((A_csr->n_rows + 1) * sizeof(size_t));
    A_csr->col_indices = malloc(A_csr->n_nz * sizeof(size_t));
    A_csr->values = malloc(A_csr->n_nz * sizeof(double));

    if (!A_csr->row_ptrs || !A_csr->col_indices || !A_csr->values) {
        printf("Loi: Khong the cap phat bo nho\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Doc row_ptrs
    for (size_t i = 0; i <= A_csr->n_rows; ++i) {
        if (fscanf(file, "%zu", &A_csr->row_ptrs[i]) != 1) {
            printf("Loi: Khong the doc row_ptrs\n");
            free_sparse_csr(A_csr);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    // Doc col_indices
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        if (fscanf(file, "%zu", &A_csr->col_indices[i]) != 1) {
            printf("Loi: Khong the doc col_indices\n");
            free_sparse_csr(A_csr);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    // Doc values
    for (size_t i = 0; i < A_csr->n_nz; ++i) {
        if (fscanf(file, "%lf", &A_csr->values[i]) != 1) {
            printf("Loi: Khong the doc values\n");
            free_sparse_csr(A_csr);
            fclose(file);
            return EXIT_FAILURE;
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int convert_csr_to_dense(const Sparse_CSR* A_csr, double** matrix) {
    size_t total_size = A_csr->n_rows * A_csr->n_cols;

    // Cap phat bo nho cho ma tran thuong
    *matrix = calloc(total_size, sizeof(double));
    if (!*matrix) {
        printf("Loi: Khong the cap phat bo nho cho ma tran thuong\n");
        return EXIT_FAILURE;
    }

    // Chuyen doi tu CSR sang ma tran thuong
    for (size_t i = 0; i < A_csr->n_rows; ++i) {
        size_t nz_start = A_csr->row_ptrs[i];
        size_t nz_end = A_csr->row_ptrs[i+1];

        for (size_t nz_id = nz_start; nz_id < nz_end; ++nz_id) {
            size_t j = A_csr->col_indices[nz_id];
            double val = A_csr->values[nz_id];
            (*matrix)[i * A_csr->n_cols + j] = val;
        }
    }

    return EXIT_SUCCESS;
}

int write_dense_matrix(const char* filename, const double* matrix, size_t n_rows, size_t n_cols) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Loi: Khong the tao file %s\n", filename);
        return EXIT_FAILURE;
    }

    // Ghi kich thuoc ma tran
    fprintf(file, "%zu %zu\n", n_rows, n_cols);

    // Ghi cac phan tu ma tran
    for (size_t i = 0; i < n_rows * n_cols; ++i) {
        fprintf(file, "%g ", matrix[i]);
        // Xuong dong sau moi hang
        if ((i + 1) % n_cols == 0) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int print_dense_matrix(const double* matrix, size_t n_rows, size_t n_cols) {
    printf("Ma tran %zu x %zu:\n", n_rows, n_cols);
    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j) {
            printf("%8.2f ", matrix[i * n_cols + j]);
        }
        printf("\n");
    }
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