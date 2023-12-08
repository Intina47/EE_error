import sys
print(sys.executable)
from transformers import pipeline

# Initialize a text-generation pipeline with the gpt-2 model
text_generator = pipeline("text-generation", model="gpt2")

# Generate a description of a really cute dog
text = text_generator("A really cute dog is", max_length=50)[0]['generated_text']

print('------------------')
print(text)
print('------------------') 