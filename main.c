/**
 * @file main.c
 * @brief Utility for encrypting, decrypting, and breaking Caesar ciphers using frequency analysis.
 * * This program provides tools to analyze text, calculate letter distributions,
 * and perform automated decryption using various distance metrics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/** @brief Size of the standard English alphabet. */
#define ALPHABET_SIZE 26
/** @brief Number of top-scoring shifts to track during frequency analysis. */
#define TOP_N 3

void clear_input_buffer();
void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]);
void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]);
double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
double euclidean_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
void encrypt(char *text, int shift);
void decrypt(char *text, int shift);
typedef double (*DistanceFunc)(const double[], const double[]);
void break_caesar_cipher(const char* text, double english_dist[ALPHABET_SIZE], int top_shifts[TOP_N], double top_distances[TOP_N], DistanceFunc dist_func);

/**
 * @brief Clears the standard input buffer (stdin).
 * * Typically called after scanf to remove the trailing newline character or
 * to handle invalid input.
 * @post The input stream is empty up to the next newline '\n' or EOF.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/**
 * @brief Reads the standard letter distribution from a text file.
 * * @param filename String containing the path to the distribution file.
 * @param distribution Double array of size ALPHABET_SIZE to store frequencies.
 * @pre The filename must point to a valid, readable file.
 * @post The distribution array is populated with values from the file (0.0 if reading fails).
 */
void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (fscanf(file, "%lf", &distribution[i]) != 1) distribution[i] = 0;
    fclose(file);
}

/**
 * @brief Calculates the normalized frequency of each letter (a-z) in a given text.
 * * @param text The null-terminated string to be analyzed.
 * @param histogram Double array to store the resulting frequencies (0.0 to 1.0).
 * @pre The text must be initialized and null-terminated.
 * @post The sum of all elements in the histogram is 1.0 (if the text contains letters).
 */
void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]) {
    int counts[ALPHABET_SIZE] = {0};
    int total_letters = 0;

    for (int i = 0; text[i] != '\0'; i++)
        if (isalpha(text[i])) {
            counts[tolower(text[i]) - 'a']++;
            total_letters++;
        }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (total_letters > 0)
            histogram[i] = (double)counts[i] / total_letters;
        else
            histogram[i] = 0;
    }
}

/**
 * @brief Calculates the Chi-Squared discrepancy between two frequency distributions.
 * * @param hist1 The observed frequency array (from the text).
 * @param hist2 The expected frequency array (e.g., standard English).
 * @return The Chi-Square distance score. A lower value indicates higher similarity.
 */
double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
        double sum = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            if (hist2[i] > 0)
                sum += pow(hist1[i] - hist2[i], 2) / hist2[i];
        return sum;
}

/**
 * @brief Calculates the straight-line (Euclidean/L2) distance between two frequency vectors.
 * * @param hist1 First frequency distribution.
 * @param hist2 Second frequency distribution.
 * @return The square root of the sum of squared differences.
 */
double euclidean_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double sum = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        sum += pow(hist1[i] - hist2[i], 2);
    }
    return sqrt(sum);
}

/**
 * @brief Measures the cosine distance between two frequency distribution vectors.
 * * @param hist1 First frequency distribution.
 * @param hist2 Second frequency distribution.
 * @return A value between 0.0 (identical) and 1.0 (completely different).
 * @post Returns 1.0 if either vector has a magnitude of zero to prevent division by zero.
 */
double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double dot_product = 0, norm1 = 0, norm2 = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        dot_product += hist1[i] * hist2[i];
        norm1 += pow(hist1[i], 2);
        norm2 += pow(hist2[i], 2);
    }
    if (norm1 == 0 || norm2 == 0) return 1.0;
    return 1.0 - (dot_product / (sqrt(norm1) * sqrt(norm2)));
}

/**
 * @brief Applies the Caesar cipher encryption by shifting letters in the alphabet.
 * * @param text The string to be encrypted.
 * @param shift The number of positions to move each letter.
 * @pre The text must be a mutable character array (not a string literal).
 * @post The text is modified in-place. Non-alphabetic characters are preserved.
 */
void encrypt(char *text, int shift) {
    // Normalize the shift in order to be between 0 and 25 for sure
    shift = (shift % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;

    for (int i = 0; text[i] != '\0'; i++)
        if (isalpha(text[i])) {
            if (islower(text[i])) {
                text[i] = (char)('a' + (text[i] - 'a' + shift) % ALPHABET_SIZE);
            }
            else if (isupper(text[i])) {
                text[i] = (char)('A' + (text[i] - 'A' + shift) % ALPHABET_SIZE);
            }
        }
}

/**
 * @brief Reverses the Caesar cipher encryption using a known shift value.
 * * @param text The encrypted string.
 * @param shift The original shift used for encryption.
 * @post The text is modified in-place to its original state.
 */
void decrypt(char *text, int shift) {
    // Decrypting is encrypting with a complementary shift
    shift = (shift % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;
    encrypt(text, ALPHABET_SIZE - shift);
}

/**
 * @brief Function pointer type for distance calculation metrics.
 */
typedef double (*DistanceFunc)(const double[], const double[]);

/**
 * @brief Performs a brute-force attack on a Caesar cipher using frequency analysis.
 * * @param text The encrypted message.
 * @param english_dist Standard English letter frequencies.
 * @param top_shifts Array to store the top best shift guesses.
 * @param top_distances Array to store the lowest distance scores corresponding to the shifts.
 * @param dist_func Pointer to the distance metric function to evaluate the distributions.
 * @post top_shifts[0] contains the shift with the absolute lowest distance score.
 */
void break_caesar_cipher(const char* text, double english_dist[ALPHABET_SIZE],
                         int top_shifts[TOP_N], double top_distances[TOP_N],
                         DistanceFunc dist_func) {
    // Initialize the top with infinity distances
    for (int i = 0; i < TOP_N; i++) {
        top_distances[i] = INFINITY;
        top_shifts[i] = -1;
    }

    // Try all possible shifts
    for (int shift = 0; shift < ALPHABET_SIZE; shift++) {
        // Working on a copy of the text in order to decrypt it without modifying it
        char *temp_text = (char *)malloc(strlen(text) + 1);
        if (temp_text == NULL) {
            printf("Memory allocation failed!\n");
            return;
        }
        strcpy(temp_text, text);

        decrypt(temp_text, shift);

        double current_hist[ALPHABET_SIZE];
        compute_histogram(temp_text, current_hist);

        // Compute the distance between the text's histogram and english's histogram
        double dist = dist_func(current_hist, english_dist);

        for (int i = 0; i < TOP_N; i++) {
            if (dist < top_distances[i]) {
                // Move the elements downwards to make space
                for (int j = TOP_N - 1; j > i; j--) {
                    top_distances[j] = top_distances[j - 1];
                    top_shifts[j] = top_shifts[j - 1];
                }
                top_distances[i] = dist;
                top_shifts[i] = shift;
                break;
            }
        }
        free(temp_text);
    }
}

/**
 * @brief Main execution function containing the interactive menu.
 * @return 0 on successful execution.
 */
int main(void) {
    double english_dist[ALPHABET_SIZE] = {0};
    read_distribution("distribution.txt", english_dist);

    char current_text[4096] = "";
    int choice = -1;

    while (choice != 0) {
        printf("1. Read a text from the keyboard\n");
        printf("2. Read a text from a file\n");
        printf("3. Encrypt the text with a specified shift\n");
        printf("4. Decrypt the text with a known shift\n");
        printf("5. Compute and display the frequency distribution\n");
        printf("6. Break the encrypted text using frequency analysis\n");
        printf("0. Exit\n");
        printf("-----------------------------------------\n");

        if (strlen(current_text) == 0)
            printf("Current text: [EMPTY]\n");
        else
            printf("Current text: [%.50s%s]\n", current_text, strlen(current_text) > 50 ? "..." : "");

        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: {
                printf("Enter the text: ");
                if (fgets(current_text, sizeof(current_text), stdin) != NULL)
                    current_text[strcspn(current_text, "\n")] = 0;
                printf("Text successfully read.\n");
                break;
            }
            case 2: {
                char filename[256];
                printf("Enter filename: ");
                if (fgets(filename, sizeof(filename), stdin) != NULL) {
                    filename[strcspn(filename, "\n")] = 0;
                    FILE *f = fopen(filename, "r");
                    if (f) {
                        size_t len = fread(current_text, 1, sizeof(current_text) - 1, f);
                        current_text[len] = '\0';
                        fclose(f);
                        printf("Text successfully loaded from %s.\n", filename);
                    }
                    else
                        printf("Error: Could not open file %s!\n", filename);
                }
                break;
            }
            case 3: {
                if (strlen(current_text) == 0) { printf("No text to encrypt!\n"); break; }
                int shift;
                printf("Enter shift value: ");
                scanf("%d", &shift);
                clear_input_buffer();
                encrypt(current_text, shift);
                printf("Text encrypted successfully!\n");
                break;
            }
            case 4: {
                if (strlen(current_text) == 0) {
                    printf("No text to decrypt!\n");
                    break;
                }
                int shift;
                printf("Enter the original shift value: ");
                scanf("%d", &shift);
                clear_input_buffer();
                decrypt(current_text, shift);
                printf("Text decrypted successfully!\n");
                break;
            }
            case 5: {
                if (strlen(current_text) == 0) {
                    printf("No text to analyze!\n");
                    break;
                }
                double hist[ALPHABET_SIZE];
                compute_histogram(current_text, hist);
                printf("\n--- Letter Frequency Distribution ---\n");
                for (int i = 0; i < ALPHABET_SIZE; i++)
                    printf("%c: %6.2f%%\n", 'A' + i, hist[i] * 100.0);
                break;
            }
            case 6: {
                if (strlen(current_text) == 0) { printf("No text to break!\n"); break; }

                printf("Select distance metric:\n");
                printf("1. Chi-Squared Distance\n");
                printf("2. Euclidean Distance\n");
                printf("3. Cosine Distance\n");
                printf("Choice: ");
                int metric_choice;
                scanf("%d", &metric_choice);
                clear_input_buffer();

                DistanceFunc func = NULL;
                if (metric_choice == 1) func = chi_squared_distance;
                else if (metric_choice == 2) func = euclidean_distance;
                else if (metric_choice == 3) func = cosine_distance;
                else {
                    printf("Invalid metric choice.\n");
                    break;
                }

                int top_shifts[TOP_N];
                double top_distances[TOP_N];

                break_caesar_cipher(current_text, english_dist, top_shifts, top_distances, func);

                printf("\n--- Most Likely Shifts ---\n");
                for (int i = 0; i < TOP_N; i++)
                    if (top_shifts[i] != -1)
                        printf("%d. Shift: %2d (Distance: %f)\n", i + 1, top_shifts[i], top_distances[i]);

                printf("\nDo you want to apply the best shift (%d) to the current text? (y/n): ", top_shifts[0]);
                char answer = getchar();
                clear_input_buffer();
                if (answer == 'y' || answer == 'Y') {
                    decrypt(current_text, top_shifts[0]);
                    printf("Text decrypted with shift %d.\n", top_shifts[0]);
                }
                break;
            }
            case 0: {
                printf("Exiting program...\n");
                break;
            }
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;

}