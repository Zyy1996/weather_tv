import sys



with open("test.html","r") as f:
    
    html_str = f.read()
    print(html_str)
    print("--------------------------------")
    html_str = html_str.replace("\"","\\\"")
    html_str = html_str.replace("\n","\\n")
    print(html_str)