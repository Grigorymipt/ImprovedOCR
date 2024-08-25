import requests
import json
import sys

url = "http://localhost:11434/api/generate"
promt="Find and complete python code with minimal corrections. Please reply with just the code."
#promt="Answer following question:"
filename = sys.argv[1]
file = open(filename, 'r')
text = file.read()

#print(f"Run ollama for {filename}")
headers = {
    "Content-Type": "application/json"
}

data = {
    "model": "llama3.1",
    "prompt": promt+text,
    "stream": False
}
response = requests.post(url, headers=headers, data=json.dumps(data))

if response.status_code == 200:
    response_text = response.text
    data = json.loads(response_text)
    actual_response = data[ "response"] 
    print(actual_response)
else:
    print("Error:", response.status_code, response.text)
