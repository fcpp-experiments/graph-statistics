import sys

# Define the number to compare with
number = int(sys.argv[2])

# Open the text file in read mode
file = open(sys.argv[1], "r")

# Create a new file to write the modified lines
new_file = open("out_" + sys.argv[1], "w")

# Loop through each line of the file
i=0
for line in file:
    if i==0:
        new_line = str(number) + "\n"
    elif i>number:
        break
    else:
        # Split the line into words
        words = line.split()
        # Create an empty list to store the filtered words
        filtered_words = []
        # Loop through each word in the line
        for word in words:
            num = int(word)
            # If the number is less than or equal to the given number, append it to the filtered list
            if num < number:
                filtered_words.append(word)
        # Join the filtered words into a new line
        new_line = " ".join(filtered_words) + "\n"
    # Write the new line to the new file
    new_file.write(new_line)
    i+=1

# Close both files
file.close()
new_file.close()
