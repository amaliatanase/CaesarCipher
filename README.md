# Caesar Cipher Breaker 🕵️‍♂️🔍

**Professional C tool** for encryption, decryption and **automatic breaking** of the Caesar Cipher using statistical frequency analysis + 3 different distance metrics.

## 🚀 Features

- 📥 **Versatile input** — keyboard or file (supports large texts)
- 🔑 **Manual encrypt/decrypt** with known shift
- 📊 **Real-time frequency analysis** — letter distribution histogram
- 🤖 **Fully automatic decryption** (no known key) using **three metrics**:
    1. **Chi-Squared** (χ²) — most accurate in most cases
    2. **Euclidean** (L₂ norm)
    3. **Cosine** distance — shape similarity

## 📋 Prerequisites
* A C compiler (e.g., **GCC**, **Clang**, or the one included in **Xcode/Code::Blocks**).
* **distribution.txt**: This file must be in the project root. It should contain 26 floating-point numbers representing the expected frequencies of letters 'A' through 'Z' in English.

## 🛠 Compilation and Execution
1. **Open your terminal/command prompt in the project directory.**
2. **Compile the source code:**
   ```bash
   gcc main.c -o caesar_breaker -lm

3. **Run the program:**
   ```bash
   ./caesar_breaker

## 📚 Documentation (Doxygen)
**The project is fully documented using Doxygen, providing detailed explanations for all functions, parameters, and algorithms used.**
**To generate the documentation:**
1. **Make sure Doxygen is installed.**
2. **Run:**
   ```bash
   doxygen Doxyfile
3. **Open the generated documentation:**
   ```bash
   html/index.html
**The documentation includes:**
- Function descriptions and parameters
- Algorithm explanations
- Code structure overview

## 🧪 Testing & Analysis
**The system was tested using texts of varying lengths and different shift values to evaluate the performance of each distance metric.**

**Methodology**
- Short texts (10–20 characters)
- Long texts (200+ characters)
- Multiple shifts (e.g., 5, 13, random values)
- All three metrics applied to each case

**Observations**
- Text length is critical:
  Short texts often lead to incorrect decryption due to unreliable frequency distributions, while long texts significantly improve accuracy.
- Chi-Squared Distance:
  Most accurate overall, especially for longer texts. Best at detecting correct shifts.
- Euclidean Distance:
  Performs well but slightly less precise than Chi-Squared.
- Cosine Distance:
  Least reliable, sometimes identifies similar patterns but fails to detect the exact shift.

**Errors & Edge Cases**
Decryption may fail in:
- Very short texts
- Repetitive texts (e.g., "AAAAAA")
- Texts with unusual letter distributions (missing common letters like 'E' or 'T')
  These cases distort frequency analysis and reduce accuracy.
## Conclusion
**Chi-Squared Distance is the most effective method in most scenarios, particularly for longer and more representative texts. However, all methods depend heavily on text quality and length. This demonstrates that frequency analysis is powerful but not infallible, especially when applied to small or non-standard datasets.**

## 👤 Author
Tanase Amalia - Elena


