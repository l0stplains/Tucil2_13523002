> Tugas Kecil 2 - IF2211 Strategi Algoritma 2025

<h3 align="center">Quadtree Image Compressor</h3>
<p align="center">
    Quadtree Image Compressor is a <i> blazingly fast </i> C++ tools for compressing image.
    <br />
    <br />
    <a href="https://github.com/l0stplains/Tucil2_13523002/releases/">Releases</a>
    ·
    <a href="https://github.com/l0stplains/Tucil2_13523002/tree/main/doc/Laporan%20Tucil2_13523002_Refki%20Alfarizi.pdf">Project Report (Bahasa Indonesia)</a>
</p>

## Project Overview

<p align="justify">
Quadtree Image Compressor is a  <i> blazingly fast </i> C++ tool designed to compress images by utilizing the quadtree data structure and divide and conquer algorithm by dividing them into smaller blocks based on color homogeneity.
</p>

<p align="justify">
This project was developed as part of my "Tugas Kecil" for the IF2211 Strategi Algoritma 2025 course at Institut Teknologi Bandung. It demonstrates the practical application of quadtree-based divide and conquer techniques in image compression.
</p>

## Requirements
- C++17
- CMake 3.10 or higher
> [!NOTE]  
> <b>For Windows users</b>, the build system prioritizes g++ (MinGW) if available. If g++ is not installed, the default system compiler (e.g., MSVC) will be used.
<br/> <b>For Linux users</b>, ensure you have a g++ (GNU) or compatible C++ compiler installed.

## Running the Application

### Windows

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/l0stplains/Tucil2_13523002.git
   cd Tucil2_13523002
   ```

2. **Build the Project:**

   Simply run the provided `build.bat` script. 
   ```bat
   build.bat
   ```

3. **Run the Executable:**

   After a successful build, the executable (`quadtree_image_compressor`) will be located in the `bin` directory relative to the project root. Run it from the Command Prompt:

   ```bat
   cd bin
   quadtree_image_compressor.exe
   ```

### Linux

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/l0stplains/Tucil2_13523002.git
   cd Tucil2_13523002
   ```

2. **Build the Project:**

   Use the provided `build.sh` script. 
   ```bash
   ./build.sh
   ```

3. **Run the Executable:**

   After the build completes, the executable will be located in the `bin` directory within the project root. Run it with:

   ```bash
   ./bin/quadtree_image_compressor
   ```


---

## Author
Refki Alfarizi (13523002)
