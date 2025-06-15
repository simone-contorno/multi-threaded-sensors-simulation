import os
import sys
import pandas as pd # type: ignore
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

def get_latest_data_folder(base_path="../data"):
    """Get the most recent data folder based on timestamp prefix."""
    data_folders = [f for f in os.listdir(base_path) if f.endswith("_data")]
    if not data_folders:
        raise Exception("No data folders found in the data directory")
    return os.path.join(base_path, sorted(data_folders)[-1])

def plot_imu_data(data_folder):
    """Plot IMU sensor data in 3D."""
    imu_file = os.path.join(data_folder, "imu.csv")
    df = pd.read_csv(imu_file)
    
    # Create 3D plot
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot only valid data points
    valid_data = df[df['valid'] == True]
    
    # Calculate the center and range for each axis
    x_data = valid_data['attitude_rate_x']
    y_data = valid_data['attitude_rate_y']
    z_data = valid_data['attitude_rate_z']
    
    # Use percentiles to remove outliers and get better scaling
    percentile_low, percentile_high = 1, 99
    x_bounds = np.percentile(x_data, [percentile_low, percentile_high])
    y_bounds = np.percentile(y_data, [percentile_low, percentile_high])
    z_bounds = np.percentile(z_data, [percentile_low, percentile_high])
    
    # Calculate the maximum range to keep aspect ratio
    max_range = max(
        x_bounds[1] - x_bounds[0],
        y_bounds[1] - y_bounds[0],
        z_bounds[1] - z_bounds[0]
    )
    
    # Calculate centers
    x_center = np.mean(x_bounds)
    y_center = np.mean(y_bounds)
    z_center = np.mean(z_bounds)
    
    scatter = ax.scatter(x_data, y_data, z_data,
                        c=valid_data['timestamp'],
                        cmap='viridis',
                        alpha=0.6)  # Add some transparency
    
    # Set equal aspect ratio and limits around center
    padding = 0.2  # 20% padding
    limit = max_range * (1 + padding) / 2
    
    # Set the limits for all axes to be equal
    ax.set_xlim(x_center - limit, x_center + limit)
    ax.set_ylim(y_center - limit, y_center + limit)
    ax.set_zlim(z_center - limit, z_center + limit)
    
    # Force equal scaling by setting axes ranges equal
    max_range = np.array([limit*2, limit*2, limit*2]).max()
    Xb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][0].flatten() + x_center
    Yb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][1].flatten() + y_center
    Zb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][2].flatten() + z_center
    
    # Comment or uncomment following both lines to test the fake bounding box:
    for xb, yb, zb in zip(Xb, Yb, Zb):
        ax.plot([xb], [yb], [zb], 'w')
    
    # Add grid for better visualization
    ax.grid(True)
    
    # Update labels with ranges
    ax.set_xlabel(f'Attitude Rate X [{x_bounds[0]:.2e}, {x_bounds[1]:.2e}]')
    ax.set_ylabel(f'Attitude Rate Y [{y_bounds[0]:.2e}, {y_bounds[1]:.2e}]')
    ax.set_zlabel(f'Attitude Rate Z [{z_bounds[0]:.2e}, {z_bounds[1]:.2e}]')
    ax.set_title('IMU Attitude Rates')
    
    plt.colorbar(scatter, label='Timestamp')
    plt.tight_layout()
    
    # Save and show plot
    save_path = os.path.join(data_folder, "imu.png")
    plt.savefig(save_path)
    plt.show()
    plt.close()
    print(f"IMU plot saved as: {save_path}")

def plot_gnss_data(data_folder):
    """Plot GNSS sensor data in 3D."""
    gnss_file = os.path.join(data_folder, "gnss.csv")
    df = pd.read_csv(gnss_file)
    
    # Create 3D plot
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot only valid data points
    valid_data = df[df['valid'] == True]
    
    # Calculate the center and range for each axis
    x_data = valid_data['pos_x']
    y_data = valid_data['pos_y']
    z_data = valid_data['pos_z']
    
    # Use percentiles to remove outliers and get better scaling
    percentile_low, percentile_high = 1, 99
    x_bounds = np.percentile(x_data, [percentile_low, percentile_high])
    y_bounds = np.percentile(y_data, [percentile_low, percentile_high])
    z_bounds = np.percentile(z_data, [percentile_low, percentile_high])
    
    # Calculate the maximum range to keep aspect ratio
    max_range = max(
        x_bounds[1] - x_bounds[0],
        y_bounds[1] - y_bounds[0],
        z_bounds[1] - z_bounds[0]
    )
    
    # Calculate centers
    x_center = np.mean(x_bounds)
    y_center = np.mean(y_bounds)
    z_center = np.mean(z_bounds)
    
    scatter = ax.scatter(x_data, y_data, z_data,
                        c=valid_data['timestamp'],
                        cmap='viridis',
                        alpha=0.6)  # Add some transparency
    
    # Set equal aspect ratio and limits around center
    padding = 0.2  # 20% padding
    limit = max_range * (1 + padding) / 2
    
    # Set the limits for all axes to be equal
    ax.set_xlim(x_center - limit, x_center + limit)
    ax.set_ylim(y_center - limit, y_center + limit)
    ax.set_zlim(z_center - limit, z_center + limit)
    
    # Force equal scaling by setting axes ranges equal
    max_range = np.array([limit*2, limit*2, limit*2]).max()
    Xb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][0].flatten() + x_center
    Yb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][1].flatten() + y_center
    Zb = 0.5*max_range*np.mgrid[-1:2:2,-1:2:2,-1:2:2][2].flatten() + z_center
    
    # Add bounding box
    for xb, yb, zb in zip(Xb, Yb, Zb):
        ax.plot([xb], [yb], [zb], 'w')
    
    # Add grid for better visualization
    ax.grid(True)
    
    # Update labels with ranges
    ax.set_xlabel(f'Position X [{x_bounds[0]:.2e}, {x_bounds[1]:.2e}]')
    ax.set_ylabel(f'Position Y [{y_bounds[0]:.2e}, {y_bounds[1]:.2e}]')
    ax.set_zlabel(f'Position Z [{z_bounds[0]:.2e}, {z_bounds[1]:.2e}]')
    ax.set_title('GNSS Positions')
    
    plt.colorbar(scatter, label='Timestamp')
    plt.tight_layout()
    
    # Save and show plot
    save_path = os.path.join(data_folder, "gnss.png")
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    plt.show()
    plt.close()
    print(f"GNSS plot saved as: {save_path}")

def main():
    # Get data folder path
    if len(sys.argv) > 1:
        data_folder = sys.argv[1]
        if not os.path.exists(data_folder):
            print(f"Error: Folder {data_folder} does not exist")
            return
    else:
        try:
            data_folder = get_latest_data_folder()
            print(f"Using most recent data folder: {data_folder}")
        except Exception as e:
            print(f"Error: {str(e)}")
            return
    
    try:
        print("Creating plots...")
        # Create plots
        plot_imu_data(data_folder)
        plot_gnss_data(data_folder)
        print(f"All plots have been saved in {data_folder}")
    except Exception as e:
        print(f"Error creating plots: {str(e)}")
        raise  # This will show the full error traceback

if __name__ == "__main__":
    main()
