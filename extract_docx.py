
import zipfile
import re
import sys
import xml.etree.ElementTree as ET

def extract_docx_text(path):
    try:
        with zipfile.ZipFile(path) as docx:
            xml_content = docx.read('word/document.xml')
            tree = ET.fromstring(xml_content)
            
            # Extract all text
            text_content = []
            for node in tree.iter():
                if node.tag.endswith('}t'):
                    if node.text:
                        text_content.append(node.text)
            
            full_text = ''.join(text_content)
            return full_text
    except Exception as e:
        return str(e)

def search_keywords(text, keywords):
    results = {}
    # Creating a simple window around keywords
    for keyword in keywords:
        matches = [m.start() for m in re.finditer(re.escape(keyword), text, re.IGNORECASE)]
        results[keyword] = []
        for m in matches:
            start = max(0, m - 100)
            end = min(len(text), m + 100)
            results[keyword].append(text[start:end])
    return results

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python extract_docx.py <path_to_docx>")
        sys.exit(1)
        
    path = sys.argv[1]
    text = extract_docx_text(path)
    
    if len(text) < 1000 and "BadZipFile" in text: # Basic error check
        print("Error reading file:", text)
        sys.exit(1)

    print(f"Extracted {len(text)} characters.")
    
    # Search for relevant terms
    keywords = ["GPIO33", "GPIO34", "GPIO35", "GPIO36", "MSDC", "MCDA", "MC0CK"]
    results = search_keywords(text, keywords)
    
    for k, v in results.items():
        print(f"\n--- Matches for {k} ---")
        for match in v[:5]: # Show top 5
            print(f"...{match}...")
