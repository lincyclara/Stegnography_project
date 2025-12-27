# Stegnography_project

# 🖼️ LSB Image Steganography using C

🔐 *Hiding information in plain sight.*

---

## 📖 About the Project
This project demonstrates **Image Steganography** using the **Least Significant Bit (LSB)** technique implemented in **C programming**.  
It allows a user to securely embed a secret text message inside a bitmap image (`.bmp`) such that the visual quality of the image remains unchanged to the human eye.

The project focuses on **low-level programming concepts**, including **bit manipulation**, **file handling**, and **memory-efficient data processing**, making it ideal for students and engineers interested in **systems programming and embedded software**.

---

## ✨ Key Highlights
- 🔏 Secure message hiding using LSB substitution  
- 🖼️ Supports bitmap (`.bmp`) images  
- ⚡ Minimal distortion in output image  
- 🧠 Demonstrates low-level C programming concepts  
- 🛠️ Modular and well-structured codebase  
- 💻 Linux / GCC compatible  

---

## 🧠 Technical Concept – LSB Steganography
In digital images, each pixel is represented using bytes.  
The **Least Significant Bit (LSB)** of a byte has minimal impact on the actual pixel value.

📌 This project:
- Replaces the LSB of image pixel bytes with secret message bits  
- Preserves image appearance  
- Enables accurate extraction of hidden data  

---

## ⚙️ How It Works
1. A cover image (`.bmp`) is selected  
2. Secret text is converted into binary format  
3. Each bit of the secret data is embedded into the LSB of image pixels  
4. A stego image is generated  
5. The hidden message is extracted by reversing the process  

---

## 🚀 How to Compile and Run

### 🔹 Compile
```bash
gcc *.c -o steganography
