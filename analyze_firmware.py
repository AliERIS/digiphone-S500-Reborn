#!/usr/bin/env python3
"""
MT6261D Firmware Reverse Engineering Tool
Extracts peripheral base addresses from binary firmware dump
"""

import sys
import struct

def find_register_addresses(filename):
    """Search for common peripheral base addresses in ARM firmware"""
    
    with open(filename, 'rb') as f:
        data = f.read()
    
    print(f"[*] Loaded firmware: {len(data)} bytes")
    print(f"[*] Searching for peripheral base addresses...\n")
    
    # Common MT6261 peripheral base addresses to search for
    # Looking for little-endian 32-bit addresses
    targets = {
        b'\x00\x00\x02\xA0': 'GPIO_BASE (0xA0020000)',
        b'\x00\x00\x70\xA0': 'PMU_BASE (0xA0700000)',
        b'\x00\x00\x18\xA0': 'LCDIF_BASE (0xA0180000)',
        b'\x00\x00\x71\xA0': 'RTC_BASE (0xA0710000)',
        b'\x00\x00\x01\xA0': 'AFE_BASE (0xA0010000)',
        b'\x00\x00\x00\x10': 'ROM_BASE (0x10000000)',
        b'\x00\x00\x00\x00': 'RAM_BASE (0x00000000)',
    }
    
    results = {}
    
    for addr_bytes, name in targets.items():
        count = data.count(addr_bytes)
        if count > 0:
            # Find first few occurrences
            positions = []
            pos = 0
            for _ in range(min(5, count)):
                pos = data.find(addr_bytes, pos)
                if pos == -1:
                    break
                positions.append(f"0x{pos:08X}")
                pos += 1
            
            results[name] = {
                'count': count,
                'positions': positions
            }
            print(f"[+] Found {name}")
            print(f"    Occurrences: {count}")
            print(f"    First locations: {', '.join(positions)}")
            print()
    
    # Search for address patterns (0xA0xxxxxx range)
    print("\n[*] Searching for all 0xA0xxxxxx addresses...")
    a0_addresses = set()
    for i in range(len(data) - 3):
        # Check for little-endian 0xA0xxxxxx
        if data[i+3] == 0xA0:
            addr = struct.unpack('<I', data[i:i+4])[0]
            if 0xA0000000 <= addr <= 0xA0FFFFFF:
                a0_addresses.add(addr)
    
    print(f"[+] Found {len(a0_addresses)} unique 0xA0xxxxxx addresses")
    print("\nMost common (likely peripheral bases):")
    
    # Count occurrences
    addr_counts = {}
    for addr in a0_addresses:
        addr_bytes = struct.pack('<I', addr)
        count = data.count(addr_bytes)
        addr_counts[addr] = count
    
    # Sort by frequency
    sorted_addrs = sorted(addr_counts.items(), key=lambda x: x[1], reverse=True)
    
    for addr, count in sorted_addrs[:20]:
        print(f"  0x{addr:08X} - {count:4d} occurrences")
    
    # Look for ARM LDR instructions pattern
    print("\n[*] Searching for ARM LDR patterns (loading addresses)...")
    ldr_patterns = []
    for i in range(0, len(data) - 8, 4):
        # ARM LDR Rx, =0xA0xxxxxx pattern
        word = struct.unpack('<I', data[i:i+4])[0]
        # Check if it's an LDR instruction (0xE59Fxxxx pattern)
        if (word & 0xFFF00000) == 0xE5900000 or (word & 0xFFF00000) == 0xE59F0000:
            # Next word might be the address
            if i + 8 < len(data):
                potential_addr = struct.unpack('<I', data[i+4:i+8])[0]
                if 0xA0000000 <= potential_addr <= 0xA0FFFFFF:
                    ldr_patterns.append((i, potential_addr))
    
    if ldr_patterns:
        print(f"[+] Found {len(ldr_patterns)} potential LDR instructions:")
        for pos, addr in ldr_patterns[:10]:
            print(f"  @ 0x{pos:08X}: LDR loading 0x{addr:08X}")
    
    return results

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python analyze_firmware.py <firmware.bin>")
        sys.exit(1)
    
    firmware_file = sys.argv[1]
    find_register_addresses(firmware_file)
