# FPGA Convolution

## Evaluation on Pre-built Image

The pre-built image is available at
[here](https://drive.google.com/file/d/1UPmeQbIADqigSYO8nZI6yS7jTgV3A7ck/view?usp=sharing).
The image is built for ZCU106 Evaluation Kit.

Please follow the steps below to evaluate the pre-built image.

### Requirements

- Zynq UltraScale+ MPSoC ZCU106 Evaluation Kit
- SD card, minimum 4GB

### Step 1 - Download and Extract the Pre-built Image

1. Download the pre-built image.
2. Clone this repository.
3. Extract the image into the "linux" directory in the repository.
   ```
   cd linux && tar -xvf <path-to-image>/optee-conv.tar.xz
   ```
   There should be two new directories, "system-conv-raw" and "system-conv-tee",
   for the versions with and without TEE enabled, respectively.

### Step 2 - Prepare the SD Card

1. Insert the SD card into your computer.
2. Format the SD card with two partitions:

   - The first partition is FAT32.
   - The second partition is ext4.

   A helper script is provided to format the SD card in this repository. Please
   replace "/dev/sdX" with the device path of your SD card.

   ```
   sudo linux/script/sdcard-partition.sh /dev/sdX
   ```

### Step 3 - Write the Image to the SD Card

Mount the two partitions of the SD card. Go to "system-conv-raw/images/linux"
OR "system-conv-tee/images/linux" directory. Copy 3 files `BOOT.BIN`,
`image.ub`, `boot.scr` into the first FAT32 partition of the SD card. Then
unarchive the `rootfs.tar.gz` into the second ext4 partition of the SD card.

You may also go to the "system-conv-raw" OR "system-conv-tee" directory and run
the following command to write the image to the SD card. Please replace
"/dev/sdX" with the device path of your SD card.

```
sudo ../script/sdcard-program.sh /dev/sdX
```

### Step 4 - Boot the ZCU106 Evaluation Kit

1. Insert the SD card into the ZCU106 Evaluation Kit.
2. Configure the switches labeled SW6 on the ZCU106 board to
   `SW6 [4:1] = OFF, OFF, OFF, ON`.
   This configuration selects the SD card as the boot device.
3. Use a Micro USB cable to connect the UART port of the ZCU106 Evaluation Kit
   to your computer.
4. Open a serial terminal emulator on the host machine and connect to the UART
   port using the following configuration: 115200 baud rate, 8 data bits, no
   parity, 1 stop bit.
5. Power on the ZCU106 board, and observe the output on the serial terminal
   emulator. The output shall present the performance counter values for the
   boot time.
6. After booting, the terminal will prompt the user to run the convolutional
   filter application. The application will give the throughput of the
   convolutional filter application.

### Step 5 - Run the Convolutional Filter Application

After booting, the terminal will prompt the user to type in the command. The
convolutional filter application will run and give the throughput by executing
the following command:

```
filter2d -i inputImage50.jpg -n 256
```

## Build the Image

### Requirements

- Vivado Design Suite 2021.2
- PetaLinux 2021.2

### Steps to Build the Image

1. Build the hardware design using Vivado Design Suite 2021.2 by running the
   following command under the root directory of the repository:
   ```
   make xsa
   ```
2. Create the PetaLinux projects by running the following commands under the
   "linux" directory:
   ```
   ./script/plnx-create-raw.sh  # for the version without TEE
   ./script/plnx-create-tee.sh  # for the version with TEE
   ```
3. Build the PetaLinux project by running the following commands under the
   ""system-conv-raw" OR "system-conv-tee"" directory:
   ```
   ../script/plnx-build.sh
   ```
4. Write the image to the SD card by running the following command under the
   "system-conv-raw" OR "system-conv-tee" directory:
   ```
   sudo ../script/sdcard-program.sh /dev/sdX
   ```
   Please replace "/dev/sdX" with the device path of your SD card.
