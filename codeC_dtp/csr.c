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

// Hàm khởi tạo CSR từ dữ liệu CSR có sẵn
int init_sparse_csr(
    size_t n_rows,
    size_t n_cols,
    size_t n_nz,
    const size_t* row_ptrs_input,
    const size_t* col_indices_input,
    const double* values_input,
    Sparse_CSR* A_csr
);

int print_sparse_csr(const Sparse_CSR* A_csr);

int matrix_vector_sparse_csr(
    const Sparse_CSR* A_csr,
    const double* vec,
    double* res
);

int free_sparse_csr(Sparse_CSR* A_csr);

int main (int argc, char** argv) {
    size_t n_rows = 5;
    size_t n_cols = 5;
    size_t n_nz = 12;

    // Dữ liệu CSR đầu vào (thay vì ma trận thường)
    size_t row_ptrs_input[] = {0, 2, 6, 9, 11, 12};
    size_t col_indices_input[] = {0, 3, 0, 1, 2, 3, 0, 3, 4, 2, 3, 4};
    double values_input[] = {1.0, 2.0, 3.0, 4.0, 2.0, 5.0, 5.0, 8.0, 17.0, 10.0, 16.0, 14.0};

    double x[] = {1, 2, 3, 4, 5};
    double Ax[5];

    // Khởi tạo ma trận CSR từ dữ liệu CSR
    Sparse_CSR A_csr;
    init_sparse_csr(n_rows, n_cols, n_nz, row_ptrs_input, col_indices_input, values_input, &A_csr);

    printf("Ma trận CSR:\n");
    print_sparse_csr(&A_csr);

    printf("\nKết quả A * x:\n");
    matrix_vector_sparse_csr(&A_csr, x, Ax);

    for (size_t i = 0; i < n_rows; ++i) {
        printf("%.2f\n", Ax[i]);
    }

    free_sparse_csr(&A_csr);
    return EXIT_SUCCESS;
}

// Hàm khởi tạo CSR từ dữ liệu CSR có sẵn
int init_sparse_csr(
    size_t n_rows,
    size_t n_cols,
    size_t n_nz,
    const size_t* row_ptrs_input,
    const size_t* col_indices_input,
    const double* values_input,
    Sparse_CSR* A_csr
) {
    A_csr->n_rows = n_rows;
    A_csr->n_cols = n_cols;
    A_csr->n_nz = n_nz;
    
    // Cấp phát bộ nhớ
    A_csr->row_ptrs = malloc((n_rows + 1) * sizeof(size_t));
    A_csr->col_indices = malloc(n_nz * sizeof(size_t));
    A_csr->values = malloc(n_nz * sizeof(double));
    
    // Kiểm tra cấp phát bộ nhớ
    if (!A_csr->row_ptrs || !A_csr->col_indices || !A_csr->values) {
        printf("Lỗi: Không thể cấp phát bộ nhớ!\n");
        return EXIT_FAILURE;
    }
    
    // Sao chép dữ liệu
    for (size_t i = 0; i <= n_rows; ++i) {
        A_csr->row_ptrs[i] = row_ptrs_input[i];
    }
    
    for (size_t i = 0; i < n_nz; ++i) {
        A_csr->col_indices[i] = col_indices_input[i];
        A_csr->values[i] = values_input[i];
    }
    
    return EXIT_SUCCESS;
}

int print_sparse_csr(const Sparse_CSR* A_csr) {
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

int free_sparse_csr(Sparse_CSR* A_csr) {
    if (A_csr->row_ptrs) free(A_csr->row_ptrs);
    if (A_csr->col_indices) free(A_csr->col_indices);
    if (A_csr->values) free(A_csr->values);
    
    A_csr->row_ptrs = NULL;
    A_csr->col_indices = NULL;
    A_csr->values = NULL;
    
    return EXIT_SUCCESS;
}