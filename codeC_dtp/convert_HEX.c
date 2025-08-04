#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *input_file, *matrix_file, *row_ptrs_file, *col_indices_file, *values_file;
    char line[10000]; // Buffer đủ lớn để chứa dòng dài
    char *token;
    int value;
    
    // Mở file input
    input_file = fopen("output_csr.txt", "r");
    if (input_file == NULL) {
        printf("Lỗi: Không thể mở file output_csr.txt\n");
        return 1;
    }
    
    // Mở các file output
    matrix_file = fopen("matrix_in4.txt", "w");
    row_ptrs_file = fopen("row_ptrs.txt", "w");
    col_indices_file = fopen("col_indices.txt", "w");
    values_file = fopen("values.txt", "w");
    
    if (!matrix_file || !row_ptrs_file || !col_indices_file || !values_file) {
        printf("Lỗi: Không thể tạo file output\n");
        if (input_file) fclose(input_file);
        if (matrix_file) fclose(matrix_file);
        if (row_ptrs_file) fclose(row_ptrs_file);
        if (col_indices_file) fclose(col_indices_file);
        if (values_file) fclose(values_file);
        return 1;
    }
    
    int line_number = 1;
    
    // Đọc từng dòng
    while (fgets(line, sizeof(line), input_file)) {
        // Loại bỏ ký tự xuống dòng
        line[strcspn(line, "\n")] = 0;
        
        // Bỏ qua dòng trống
        if (strlen(line) == 0) continue;
        
        switch (line_number) {
            case 1: // Dòng đầu tiên - matrix_in4.txt (giữ nguyên dạng thập phân)
                token = strtok(line, " ");
                while (token != NULL) {
                    fprintf(matrix_file, "%s\n", token);
                    token = strtok(NULL, " ");
                }
                break;
                
            case 2: // Dòng thứ hai - row_ptrs.txt (chuyển sang HEX)
                token = strtok(line, " ");
                while (token != NULL) {
                    value = atoi(token);
                    fprintf(row_ptrs_file, "%X\n", value);
                    token = strtok(NULL, " ");
                }
                break;
                
            case 3: // Dòng thứ ba - col_indices.txt (chuyển sang HEX)
                token = strtok(line, " ");
                while (token != NULL) {
                    value = atoi(token);
                    fprintf(col_indices_file, "%X\n", value);
                    token = strtok(NULL, " ");
                }
                break;
                
            case 4: // Dòng thứ tư - values.txt (chuyển sang HEX)
                token = strtok(line, " ");
                while (token != NULL) {
                    value = atoi(token);
                    fprintf(values_file, "%X\n", value);
                    token = strtok(NULL, " ");
                }
                break;
        }
        
        line_number++;
    }
    
    // Đóng tất cả file
    fclose(input_file);
    fclose(matrix_file);
    fclose(row_ptrs_file);
    fclose(col_indices_file);
    fclose(values_file);
    
    printf("Chuyển đổi thành công!\n");
    printf("Đã tạo 4 file:\n");
    printf("- matrix_in4.txt (thập phân)\n");
    printf("- row_ptrs.txt (HEX)\n");
    printf("- col_indices.txt (HEX)\n");
    printf("- values.txt (HEX)\n");
    
    return 0;
}