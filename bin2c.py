#!/usr/bin/env python3
"""
Convert binary firmware files to C arrays for embedding in flash
Usage: python bin2c.py <input.bin> <output_name>
Example: python bin2c.py ARM_BL.BIN arm_bl
"""

import sys
import os

def bin2c(input_file, output_name):
    """Convert binary file to C array"""
    
    if not os.path.exists(input_file):
        print(f"Error: File '{input_file}' not found")
        return False
    
    file_size = os.path.getsize(input_file)
    print(f"Converting {input_file} ({file_size} bytes) to C array...")
    
    with open(input_file, 'rb') as f:
        data = f.read()
    
    # Generate C header
    header_name = f"{output_name}_data.h"
    with open(header_name, 'w') as h:
        h.write(f"/* Auto-generated from {input_file} */\n")
        h.write(f"#ifndef _{output_name.upper()}_DATA_H_\n")
        h.write(f"#define _{output_name.upper()}_DATA_H_\n\n")
        h.write(f"#include <stdint.h>\n\n")
        h.write(f"#define {output_name}_size {file_size}\n\n")
        h.write(f"extern const uint8_t {output_name}_data[{file_size}];\n\n")
        h.write(f"#endif /* _{output_name.upper()}_DATA_H_ */\n")
    
    # Generate C source
    source_name = f"{output_name}_data.c"
    with open(source_name, 'w') as c:
        c.write(f"/* Auto-generated from {input_file} */\n")
        c.write(f"#include \"{header_name}\"\n\n")
        c.write(f"const uint8_t {output_name}_data[{file_size}] = {{\n")
        
        # Write data in rows of 16 bytes
        for i in range(0, len(data), 16):
            chunk = data[i:i+16]
            hex_str = ', '.join(f'0x{b:02X}' for b in chunk)
            c.write(f"    {hex_str},\n")
        
        c.write("};\n")
    
    print(f"Generated: {header_name}, {source_name}")
    return True

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python bin2c.py <input.bin> <output_name>")
        print("Example: python bin2c.py ARM_BL.BIN arm_bl")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_name = sys.argv[2]
    
    if bin2c(input_file, output_name):
        print("Conversion successful!")
    else:
        sys.exit(1)
