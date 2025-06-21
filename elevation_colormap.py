import numpy as np
import matplotlib.colors as mcolors

def create_realistic_elevation_colormap():
    """
    Create a realistic elevation colormap:
    - Dark blue to bright blue for negative elevations (underwater)
    - Bright blue to green to white for positive elevations (land)
    """
    # Define elevation breakpoints and corresponding colors
    colors = [
        '#000080',  # Dark blue (deep water)
        '#0000FF',  # Bright blue (shallow water)
        '#00FFFF',  # Cyan (sea level)
        '#00FF00',  # Green (lowlands)
        '#FFFF00',  # Yellow (hills)
        '#FFA500',  # Orange (mountains)
        '#FFFFFF'   # White (peaks)
    ]
    
    # Create colormap
    n_bins = 256
    cmap = mcolors.LinearSegmentedColormap.from_list('realistic_elevation', colors, N=n_bins)
    
    return cmap

def apply_elevation_coloring(elevation_data, ax, title, vmin=None, vmax=None):
    """
    Apply realistic elevation coloring to terrain data
    
    Args:
        elevation_data: 2D numpy array of elevation values
        ax: matplotlib axis to plot on
        title: Plot title
        vmin, vmax: Optional min/max values for color scaling
    """
    if vmin is None:
        vmin = np.min(elevation_data)
    if vmax is None:
        vmax = np.max(elevation_data)
        
    # Create realistic elevation colormap
    cmap = create_realistic_elevation_colormap()
    
    # Display the terrain with proper coloring
    im = ax.imshow(elevation_data, cmap=cmap, origin='lower', vmin=vmin, vmax=vmax)
    ax.set_title(title)
    
    # Add colorbar with proper labeling
    cbar = ax.figure.colorbar(im, ax=ax, label='Elevation (m)')
    
    # Add sea level line marker on colorbar if data spans sea level
    if vmin < 0 < vmax:
        # Calculate sea level position on colorbar
        sea_level_pos = -vmin / (vmax - vmin)
        cbar.ax.axhline(y=sea_level_pos, color='cyan', linewidth=2, alpha=0.8)
        cbar.ax.text(1.1, sea_level_pos, 'Sea Level', va='center', ha='left', 
                    color='cyan', fontweight='bold', fontsize=8)
    
    return im, cbar
