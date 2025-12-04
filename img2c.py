#!/usr/bin/env python3
"""
Convert image to RGB565 C array for DZ09 boot logo
Usage: python img2c.py <input.png> <output_name>
Requirements: pip install pillow
"""

import sys
from PIL import Image

def rgb888_to_rgb565(r, g, b):
    """Convert RGB888 to RGB565"""
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    return (r5 << 11) | (g6 << 5) | b5

def image_to_c_array(input_file, output_name, target_width=240, target_height=320):
    """Convert image to RGB565 C array"""
    
    try:
        img = Image.open(input_file)
    except Exception as e:
        print(f"Error opening image: {e}")
        return False
    
    # Convert to RGB if needed
    if img.mode != 'RGB':
        img = img.convert('RGB')
    
    # Resize to target size
    img = img.resize((target_width, target_height), Image.Resampling.LANCZOS)
    
    # Get pixels
    pixels = img.load()
    
    print(f"Converting {input_file} ({target_width}x{target_height}) to RGB565...")
    
    # Generate header
    header_name = f"{output_name}_logo.h"
    with open(header_name, 'w') as h:
        h.write(f"/* Auto-generated boot logo from {input_file} */\n")
        h.write(f"#ifndef _{output_name.upper()}_LOGO_H_\n")
        h.write(f"#define _{output_name.upper()}_LOGO_H_\n\n")
        h.write(f"#include <stdint.h>\n\n")
        h.write(f"#define LOGO_WIDTH  {target_width}\n")
        h.write(f"#define LOGO_HEIGHT {target_height}\n\n")
        h.write(f"extern const uint16_t {output_name}_logo_data[{target_width * target_height}];\n\n")
        h.write(f"#endif /* _{output_name.upper()}_LOGO_H_ */\n")
    
    # Generate source
    source_name = f"{output_name}_logo.c"
    with open(source_name, 'w') as c:
        c.write(f"/* Auto-generated boot logo from {input_file} */\n")
        c.write(f"#include \"{header_name}\"\n\n")
        c.write(f"const uint16_t {output_name}_logo_data[{target_width * target_height}] = {{\n")
        
        # Convert pixels to RGB565
        count = 0
        for y in range(target_height):
            for x in range(target_width):
                r, g, b = pixels[x, y][:3]  # Get RGB, ignore alpha if present
                rgb565 = rgb888_to_rgb565(r, g, b)
                
                if count % 12 == 0:
                    c.write("    ")
                
                c.write(f"0x{rgb565:04X}")
                
                if count < (target_width * target_height - 1):
                    c.write(", ")
                
                if count % 12 == 11:
                    c.write("\n")
                
                count += 1
        
        c.write("\n};\n")
    
    print(f"Generated: {header_name}, {source_name}")
    print(f"Size: {target_width * target_height * 2} bytes ({(target_width * target_height * 2) / 1024:.1f} KB)")
    return True

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python img2c.py <input.png> [output_name]")
        print("Example: python img2c.py logo.png boot")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_name = sys.argv[2] if len(sys.argv) > 2 else "boot"
    
    if image_to_c_array(input_file, output_name):
        print("Conversion successful!")
    else:
        sys.exit(1)
