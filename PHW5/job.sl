#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=16GB
#SBATCH --time=1:00:00
#SBATCH --partition=gpu 
#SBATCH --output=gpujob.out
#SBATCH --gres=gpu:v100:1

module purge
module load legacy/CentOS7
module load gcc/9.2.0
module load nvidia-hpc-sdk

echo "==================== Running p1_memory ===================="
./p1_memory

echo "==========================================================="

echo "==================== Running p2_memory ===================="
./p2_memory

echo "==========================================================="

echo "==================== Running p1_async ====================="
echo "=== nStreams = 1 ==="
./p1_async 1
echo "=== nStreams = 4 ==="
./p1_async 4
echo "=== nStreams = 16 ==="
./p1_async 16

echo "==========================================================="

echo "==================== Running p2_async ====================="
echo "=== nStreams = 1 ==="
./p2_async 1
echo "=== nStreams = 4 ==="
./p2_async 4
echo "=== nStreams = 16 ==="
./p2_async 16