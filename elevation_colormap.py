import numpy as np
import matplotlib.colors as mcolors
import matplotlib.pyplot as plt

# VOXEL CASTLE STANDARDIZED ELEVATION COLORING SYSTEM
# Game world elevation range: -2048m to +2048m
WORLD_MIN_ELEVATION = -2048.0
WORLD_MAX_ELEVATION = +2048.0
SEA_LEVEL = 0.0

def create_voxel_castle_elevation_colormap():
    """
    Create the standardized Voxel Castle elevation colormap with clear land/sea distinction:
    
    Elevation Zones:
    -2048m to    0m: Ocean (dark blue → bright blue)
       0m to +200m: Lowlands (green)
    +200m to +800m: Hills (yellow → orange)
    +800m to +2048m: Mountains (brown → white)
    
    Returns:
        matplotlib colormap optimized for -2048m to +2048m range
    """
    # Define precise color zones with realistic transitions
    elevation_points = [
        -2048,  # Deep ocean floor
        -1000,  # Deep ocean
        -200,   # Continental shelf
        0,      # SEA LEVEL (critical boundary)
        200,    # Lowlands
        800,    # Hills
        1400,   # Mountains
        2048    # Mountain peaks
    ]
    
    colors = [
        '#000033',  # Deep ocean floor (very dark blue)
        '#000080',  # Deep ocean (dark blue)
        '#0066CC',  # Continental shelf (medium blue)
        '#0099FF',  # Sea level (bright blue)
        '#228B22',  # Lowlands (forest green)
        '#FFFF00',  # Hills (yellow)
        '#FF8C00',  # Mountains (dark orange)
        '#FFFFFF'   # Peaks (white)
    ]
    
    # Normalize elevation points to 0-1 range for colormap
    normalized_points = [(e - WORLD_MIN_ELEVATION) / (WORLD_MAX_ELEVATION - WORLD_MIN_ELEVATION) 
                        for e in elevation_points]
    
    # Create segmented colormap
    color_segments = list(zip(normalized_points, colors))
    cmap = mcolors.LinearSegmentedColormap.from_list('voxel_castle_elevation', color_segments, N=512)
    
    return cmap

def apply_standardized_elevation_coloring(elevation_data, ax, title, show_sea_level_marker=True):
    """
    Apply standardized Voxel Castle elevation coloring to terrain data.
    
    Args:
        elevation_data: 2D numpy array of elevation values in meters
        ax: matplotlib axis to plot on
        title: Plot title
        show_sea_level_marker: Whether to show sea level marker on colorbar
        
    Returns:
        tuple: (image, colorbar) matplotlib objects
    """
    # Always use absolute world range for consistency
    vmin = WORLD_MIN_ELEVATION
    vmax = WORLD_MAX_ELEVATION
    
    # Get standardized colormap
    cmap = create_voxel_castle_elevation_colormap()
    
    # Display terrain with standardized coloring
    im = ax.imshow(elevation_data, cmap=cmap, origin='lower', vmin=vmin, vmax=vmax)
    ax.set_title(title)
    
    # Add standardized colorbar
    cbar = ax.figure.colorbar(im, ax=ax, label='Elevation (m)')
    
    # Add critical elevation markers
    if show_sea_level_marker:
        # Sea level marker (critical land/sea boundary)
        sea_level_norm = (SEA_LEVEL - vmin) / (vmax - vmin)
        cbar.ax.axhline(y=sea_level_norm, color='cyan', linewidth=3, alpha=0.9)
        cbar.ax.text(1.15, sea_level_norm, 'Sea Level (0m)', va='center', ha='left',
                    color='cyan', fontweight='bold', fontsize=9)
        
        # Additional reference markers
        markers = [
            (-1000, 'Deep Ocean', 'lightblue'),
            (200, 'Lowlands', 'lightgreen'),
            (800, 'Hills', 'orange'),
            (1400, 'Mountains', 'brown')
        ]
        
        for elevation, label, color in markers:
            if vmin <= elevation <= vmax:
                norm_pos = (elevation - vmin) / (vmax - vmin)
                cbar.ax.axhline(y=norm_pos, color=color, linewidth=1, alpha=0.6, linestyle='--')
    
    # Set colorbar ticks to meaningful elevations
    tick_elevations = [-2000, -1000, 0, 500, 1000, 1500, 2000]
    tick_positions = [(e - vmin) / (vmax - vmin) for e in tick_elevations]
    cbar.set_ticks(tick_positions)
    cbar.set_ticklabels([f'{e}m' for e in tick_elevations])
    
    return im, cbar

def get_elevation_stats(elevation_data):
    """
    Get standardized elevation statistics for terrain data.
    
    Args:
        elevation_data: 2D numpy array of elevation values
        
    Returns:
        dict: Statistics including land/sea percentages
    """
    stats = {
        'min_elevation': np.min(elevation_data),
        'max_elevation': np.max(elevation_data),
        'mean_elevation': np.mean(elevation_data),
        'sea_level_percentage': np.sum(elevation_data <= SEA_LEVEL) / elevation_data.size * 100,
        'land_percentage': np.sum(elevation_data > SEA_LEVEL) / elevation_data.size * 100,
        'deep_ocean_percentage': np.sum(elevation_data < -200) / elevation_data.size * 100,
        'mountain_percentage': np.sum(elevation_data > 800) / elevation_data.size * 100
    }
    return stats

# Legacy compatibility function (redirects to new system)
def apply_elevation_coloring(elevation_data, ax, title, vmin=None, vmax=None):
    """
    Legacy compatibility function - redirects to standardized system.
    Note: vmin/vmax parameters are ignored to ensure consistency.
    """
    print("WARNING: Using legacy apply_elevation_coloring() - redirecting to standardized system")
    return apply_standardized_elevation_coloring(elevation_data, ax, title)
