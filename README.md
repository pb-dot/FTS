# Highly Available Distributed Tftp File Transfer System (FTS)

This project implements a **Highly Available Distributed File Transfer System** that ensures efficient and reliable file transfers through *Tftp*. The system is designed with a layered architecture for modularity and scalability.

## 📂 Project Components

The project consists of the following key components:

### 1. **TFTP Server (Layer-3)**
   - A TFTP server that handles the file transfer requests.
   - Each server is capable of serving multiple client tftp packets.

### 2. **Load Balancer (Layer-2)**
   - Distributes incoming requests to the TFTP servers efficiently.
   - Maintains fault tolerance by redistributing the load of the down server among the rest active servers.

### 3. **Switch Emulator (Layer-1)**
   - Simulates a switch that connects the clients, load balancer.
   - Switches between Active and Backup LoadBalancer.

### 4. **TFTP Client (Layer-0)**
   - An Industry Standard TFTP client that interacts with the system to perform file transfers.
   - **Note:** Users can either use the TFTP client provided in this repository or bring their own standard TFTP client.

## 🖼️ System Architecture

The complete architecture of the system is illustrated in the image below. You can find this image in the `Readme_utils` folder of this repository.

![System Architecture](Readme_utils/system_architecture.png)

## 🚀 Features

### 1. **Highly Available**
   - No effect on the ongoing file tansfer if any one or more of the tftp servers(Layer-3) goes down
   - No effect on the ongoing file transfer if any one of the Load Balancer goes down [immediately the other takes its place]

### 2. **Highly Scalable**
   - Can serve any number of Tftp clients simultaneously
   - Can add any number of Tftp servers to the system

### 3. **Highly Modular**
   - Each instance of any Layer can be present on a unique IP
   - Following Layered Architecture increases Modularity

