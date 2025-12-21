
import zipfile
import re
import sys
import xml.etree.ElementTree as ET

def extract_docx_text(path):
    try:
        with zipfile.ZipFile(path) as docx:
            xml_content = docx.read('word/document.xml')
            tree = ET.fromstring(xml_content)
            text_content = []
            for node in tree.iter():
                if node.tag.endswith('}t'):
                    if node.text:
                        text_content.append(node.text)
            return ''.join(text_content)
    except Exception as e:
        return ""

path = "MT6261D_dATASHEET.docx"
text = extract_docx_text(path)

# Search for Memory Map related keywords
patterns = ["Memory Map", "Base Address", "Global Memory", "0x1000", "0x1100", "0x1800"]
for p in patterns:
    print(f"--- Searching for {p} ---")
    matches = [m.start() for m in re.finditer(re.escape(p), text, re.IGNORECASE)]
    for m in matches[:5]: 
        start = max(0, m - 100)
        end = min(len(text), m + 100)
        snippet = text[start:end].replace('\n', ' ')
        print(f"...{snippet}...")
