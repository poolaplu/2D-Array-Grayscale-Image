#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_HEIGHT 1000
#define MAX_WIDTH 1000

// Declare the external assembly function
// int* inputImage - stored in rcx
// double* outputImage - stored in rdx
// int width - stored in r8
// int height - stored in r9
extern void imgCvtGrayIntToDouble(int* inputImage, double* outputImage, int width, int height);

// Gets user input for pixel values
void getInputFromUser(int* inputImage, int width, int height) {
    printf("Enter the pixel values (0-255):\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            scanf("%d", &inputImage[i * width + j]);
        }
    }
}

// Prints converted grayscale image
void printImageDouble(double* outputImage, int width, int height) {
    printf("Converted Grayscale Image:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%.2lf ", outputImage[i * width + j]);
        }
        printf("\n");
    }
}

// For testing only
int randomPixel() {
    return rand() % 256;
}

// Populate the image with random pixel values
void generateRandomImage(int* inputImage, int width, int height) {
    for (int i = 0; i < height * width; i++) {
        inputImage[i] = randomPixel();
    }
}

// Prints inputImage (int array)
void printInputImage(int* inputImage, int width, int height) {
    printf("Generated Image:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%3d ", inputImage[i * width + j]);
        }
        printf("\n");
    }
}

// C version of ASM function
void C_imgCvtGrayIntToDouble(int* inputImage, double* outputImage, int width, int height) {
    // Iterate over each row
    for (int i = 0; i < height; i++) {
        // Iterate over each column in the row
        for (int j = 0; j < width; j++) {
            // Get the pixel value from the input image
            int pixelValue = inputImage[i * width + j];

            // Perform the conversion (divide by 255.0)
            double convertedValue = pixelValue / 255.0;

            // Store the converted value in the output image
            outputImage[i * width + j] = convertedValue;
        }
    }
}

int main() {
    srand(time(NULL)); // Seed for random number generator
    int width, height;

    // Get user input for width and height
    printf("Enter width and height: ");
    scanf("%d %d", &width, &height);

    // Error handling
    if (height > MAX_HEIGHT || width > MAX_WIDTH) {
        printf("Invalid dimension size. Maximum allowed dimensions are %dx%d.\n", MAX_HEIGHT, MAX_WIDTH);
        return 1;
    }

    // Allocate memory for inputImage and outputImage arrays
    int totalElements = height * width;

    int* inputImage = (int*)malloc(totalElements * sizeof(int));
    double* outputImage = (double*)malloc(totalElements * sizeof(double));

    if (!inputImage || !outputImage) {
        printf("Memory allocation failed.\n");
        free(inputImage);
        free(outputImage);
        return 1;
    }

    int choice = 0;
    // Prompt the user to choose the image input method
    printf("Choose image input method:\n");
    printf("1. Input image manually\n");
    printf("2. Generate random image\n");
    printf("3. Test runtime\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            // Get pixel values from user
            getInputFromUser(inputImage, width, height);
            imgCvtGrayIntToDouble(inputImage, outputImage, width, height); // Convert grayscale image using assembly function
            printImageDouble(outputImage, width, height);
            break;
        case 2:
            // Generate random image
            generateRandomImage(inputImage, width, height);
            printInputImage(inputImage, width, height);
            imgCvtGrayIntToDouble(inputImage, outputImage, width, height); // Convert grayscale image using assembly function
            printImageDouble(outputImage, width, height);
            break;
        case 3:
            // Test runtime for ASM
            struct timespec start, end;  // Define timespec for measuring time in nanoseconds
            long long totalTime = 0;    // Total time in nanoseconds
            long long averageRuntime = 0; 
            printf("\nFor ASM Runtime:\n");
            for (int i = 1; i <= 30; i++) {
                generateRandomImage(inputImage, width, height);
                clock_gettime(CLOCK_MONOTONIC, &start); // Record the start time for the grayscale conversion function
                imgCvtGrayIntToDouble(inputImage, outputImage, width, height); // Convert grayscale image using ASM function
                clock_gettime(CLOCK_MONOTONIC, &end); // Record the end time after the grayscale conversion
                
                // Calculate the elapsed time in nanoseconds
                long long timeTaken = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
                totalTime += timeTaken;
                printf("Time taken for ASM grayscale conversion #%d: %lld nanoseconds\n", i, timeTaken);
            }
            averageRuntime = totalTime / 30;
            printf("Average ASM Runtime after 30 conversions: %lld nanoseconds\n\n", averageRuntime);

            // Test runtime for C
            totalTime = 0;  // Reset totalTime for C function runtime
            averageRuntime = 0;  // Reset averageRuntime for C function runtime
            printf("For C Runtime:\n");
            for (int i = 1; i <= 30; i++) {
                generateRandomImage(inputImage, width, height);
                clock_gettime(CLOCK_MONOTONIC, &start); // Record the start time for the grayscale conversion function
                C_imgCvtGrayIntToDouble(inputImage, outputImage, width, height); // Convert grayscale image using C function
                clock_gettime(CLOCK_MONOTONIC, &end); // Record the end time after the grayscale conversion
                
                // Calculate the elapsed time in nanoseconds
                long long timeTaken = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
                totalTime += timeTaken;
                printf("Time taken for C grayscale conversion #%d: %lld nanoseconds\n", i, timeTaken);
            }
            averageRuntime = totalTime / 30;
            printf("Average C Runtime after 30 conversions: %lld nanoseconds\n", averageRuntime);
            break;
        default:
            printf("Invalid choice.\n");
            free(inputImage);
            free(outputImage);
            return 1;
    }

    // Free allocated memory
    free(inputImage);
    free(outputImage);

    return 0;
}
