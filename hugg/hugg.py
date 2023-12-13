from transformers import pipeline

# Sample headlines
# read from file
headlines = []
with open('headlines.txt', 'r') as f:
    headlines = f.readlines()
    
# remove \n
headlines = [h.strip() for h in headlines]

# Summarization and text generation pipelines
summarizer = pipeline("summarization")
text_generator = pipeline("text-generation")

# Perform summarization and text generation for each headline
for headline in headlines:
    # Perform summarization
    summary = summarizer(headline, max_length=50, min_length=10, length_penalty=2.0, num_beams=4)[0]["summary_text"]

    # Perform text generation
    generated_text = text_generator(f"Generate news about: {headline}", max_length=50, num_beams=4)[0]["generated_text"]

    # Print the results
    print(f"Headline: {headline}")
    print(f"Summary: {summary}")
    print(f"Generated Text: {generated_text}")
    print("=" * 50)
