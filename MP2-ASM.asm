section .data
    divisor dq 255.0

section .text
bits 64
default rel
global imgCvtGrayIntToDouble

imgCvtGrayIntToDouble:
    ; imgCvtGrayIntToDouble(int* inputImage, double* outputImage, int width, int height)
    ; rcx - address of the input image array (inputImage)
    ; rdx - address of the output image array (outputImage)
    ; r8  - width of the image (width)
    ; r9  - height of the image (height)
    
    ; r10 - pointer to current input pixel (int)
    ; r11 - pointer to current output pixel (double)
    ; r12 - row counter (i)
    ; r13 - column counter (j)
    ; r14 - current row

    ; xmm0 - holds the converted pixel value
    ; xmm1 - stores 255.0

    ; initialize row and column counter
    mov r12, 0 ; row (i)
    mov r13, 0 ; col (j)
    
row_loop:
    ; if i == height, finish looping
    cmp r12, r9
    je done_looping  
                  
    ; get current row (current row = i * width)                
    mov r14, r12            
    imul r14, r8            
    
    ; pointer to current row for int array (inputImage)
    lea r10, [rcx + r14*4]
    ; pointer to current row for double array (outputImage)
    lea r11, [rdx + r14*8]

    ; reset column index (j)
    mov r13, 0

col_loop:
    ; if j == width, move to next row
    cmp r13, r8
    je next_row
    
    ; get pixel value from the inputImage
    mov eax, dword [r10 + r13*4]  
    
    ; convert the integer to a double (xmm0 = (double)eax)
    cvtsi2sd xmm0, rax      
    
    ; divide xmm0 to 255.0 (stored in xmm1)
    movq xmm1, qword [divisor]
    divsd xmm0, xmm1

    ; store new pixel value in the outputImage
    movsd qword [r11 + r13*8], xmm0  

    ; loop until all columns in row are done
    inc r13
    jmp col_loop

next_row:
    inc r12                 ; row counter (i)
    jmp row_loop

done_looping:
    xor rax, rax
    ret
