#include <stdio.h>
#include <stdlib.h>

// Ham doc ma tran tu file
int read_matrix_from_file(const char* filename, int*** matrix, int* rows, int* cols);

// Ham doc vector tu file
int read_vector_from_file(const char* filename, int** vector, int expected_size);

// Ham nhan ma tran voi vector
int matrix_vector_multiply(int** matrix, int* vector, int* result, int rows, int cols);

// Ham ghi ket qua ra file
int write_result_to_file(const char* filename, int* result, int size);

// Ham giai phong bo nho ma tran
void free_matrix(int** matrix, int rows);

// Ham in ma tran
void print_matrix(int** matrix, int rows, int cols);

// Ham in vector
void print_vector(int* vector, int size, const char* name);

int main(int argc, char** argv) {
    const char* matrix_file = "matrix_dense.txt";
    const char* vector_file = "vector.txt";
    const char* output_file = "output_calculate_matrixDense.txt";
    
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
    
    int** matrix = NULL;
    int* vector = NULL;
    int* result = NULL;
    int rows, cols;
    
    printf("Dang doc ma tran tu file: %s\n", matrix_file);
    
    // Doc ma tran tu file
    if (read_matrix_from_file(matrix_file, &matrix, &rows, &cols) != 0) {
        printf("Loi: Khong the doc ma tran tu file!\n");
        return 1;
    }
    
    printf("Doc ma tran thanh cong!\n");
    printf("Ma tran co %d hang va %d cot.\n", rows, cols);
    
    printf("\nDang doc vector tu file: %s\n", vector_file);
    
    // Doc vector tu file
    if (read_vector_from_file(vector_file, &vector, cols) != 0) {
        printf("Loi: Khong the doc vector tu file!\n");
        free_matrix(matrix, rows);
        return 1;
    }
    
    printf("Doc vector thanh cong!\n");
    
    // Cap phat bo nho cho ket qua
    result = (int*)malloc(rows * sizeof(int));
    if (!result) {
        printf("Loi: Khong the cap phat bo nho cho ket qua!\n");
        free_matrix(matrix, rows);
        free(vector);
        return 1;
    }
    
    // Hien thi ma tran da nhap
    printf("\nMa tran da nhap:\n");
    print_matrix(matrix, rows, cols);
    
    // Hien thi vector da nhap
    printf("\nVector da nhap:\n");
    print_vector(vector, cols, "vector");
    
    // Thuc hien phep nhan ma tran voi vector
    printf("\nDang thuc hien phep nhan ma tran voi vector...\n");
    if (matrix_vector_multiply(matrix, vector, result, rows, cols) != 0) {
        printf("Loi: Khong the thuc hien phep nhan!\n");
        free_matrix(matrix, rows);
        free(vector);
        free(result);
        return 1;
    }
    
    // Hien thi ket qua tren man hinh
    printf("\nVector ket qua sau khi nhan:\n");
    print_vector(result, rows, "ket qua");
    
    // Ghi ket qua ra file
    printf("\nDang ghi ket qua ra file: %s\n", output_file);
    if (write_result_to_file(output_file, result, rows) != 0) {
        printf("Loi: Khong the ghi ket qua ra file!\n");
        free_matrix(matrix, rows);
        free(vector);
        free(result);
        return 1;
    }
    
    printf("Da ghi ket qua vao file %s\n", output_file);
    printf("Hoan thanh!\n");
    
    // Giai phong bo nho
    free_matrix(matrix, rows);
    free(vector);
    free(result);
    
    return 0;
}

int read_matrix_from_file(const char* filename, int*** matrix, int* rows, int* cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file ma tran %s\n", filename);
        return 1;
    }
    
    // Doc so hang va so cot cua ma tran
    if (fscanf(file, "%d %d", rows, cols) != 2) {
        printf("Loi: Khong the doc kich thuoc ma tran!\n");
        fclose(file);
        return 1;
    }
    
    // Cap phat bo nho cho ma tran
    *matrix = (int**)malloc(*rows * sizeof(int*));
    if (!*matrix) {
        printf("Loi: Khong the cap phat bo nho cho ma tran!\n");
        fclose(file);
        return 1;
    }
    
    for (int i = 0; i < *rows; i++) {
        (*matrix)[i] = (int*)malloc(*cols * sizeof(int));
        if (!(*matrix)[i]) {
            printf("Loi: Khong the cap phat bo nho cho hang %d!\n", i);
            // Giai phong bo nho da cap phat
            for (int j = 0; j < i; j++) {
                free((*matrix)[j]);
            }
            free(*matrix);
            fclose(file);
            return 1;
        }
    }
    
    // Doc cac phan tu cua ma tran tu file
    printf("Dang doc cac phan tu ma tran...\n");
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            if (fscanf(file, "%d", &(*matrix)[i][j]) != 1) {
                printf("Loi: Khong the doc phan tu ma tran[%d][%d]!\n", i, j);
                free_matrix(*matrix, *rows);
                fclose(file);
                return 1;
            }
        }
    }
    
    fclose(file);
    return 0;
}

int read_vector_from_file(const char* filename, int** vector, int expected_size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Loi: Khong the mo file vector %s\n", filename);
        return 1;
    }
    
    // Cap phat bo nho cho vector
    *vector = (int*)malloc(expected_size * sizeof(int));
    if (!*vector) {
        printf("Loi: Khong the cap phat bo nho cho vector!\n");
        fclose(file);
        return 1;
    }
    
    // Doc cac phan tu cua vector tu file
    printf("Dang doc cac phan tu vector...\n");
    for (int i = 0; i < expected_size; i++) {
        if (fscanf(file, "%d", &(*vector)[i]) != 1) {
            printf("Loi: Khong the doc phan tu vector[%d]!\n", i);
            printf("Kiem tra lai kich thuoc vector trong file!\n");
            free(*vector);
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

int matrix_vector_multiply(int** matrix, int* vector, int* result, int rows, int cols) {
    // Thuc hien phep nhan ma tran voi vector
    for (int i = 0; i < rows; i++) {
        result[i] = 0; // Khoi tao gia tri ban dau
        for (int j = 0; j < cols; j++) {
            result[i] += matrix[i][j] * vector[j];
        }
    }
    return 0;
}

int write_result_to_file(const char* filename, int* result, int size) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Loi: Khong the tao file %s\n", filename);
        return 1;
    }
    
    fprintf(file, "Vector ket qua sau khi nhan ma tran voi vector:\n");
    fprintf(file, "Kich thuoc ket qua: %d\n", size);
    fprintf(file, "----------------------------------------\n");
    
    for (int i = 0; i < size; i++) {
        fprintf(file, "result[%d] = %d\n", i, result[i]);
    }
    
    fclose(file);
    return 0;
}

void free_matrix(int** matrix, int rows) {
    if (matrix) {
        for (int i = 0; i < rows; i++) {
            if (matrix[i]) {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

void print_matrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void print_vector(int* vector, int size, const char* name) {
    printf("Cac phan tu cua %s:\n", name);
    for (int i = 0; i < size; i++) {
        printf("%s[%d] = %d\n", name, i, vector[i]);
    }
}