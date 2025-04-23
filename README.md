## Usage Details

1. Generate random student data
Creates a synthetic data.csv file with student information.
You specify how many students to generate (e.g., 1000 students).
2. Compare matcher performances
Loads students from data.csv.
Builds both matchers (HashMatcher and GraphMatcher).
Measures runtime and Jaccard similarity between the two matchers.
3. Search matches for a single student
Enter a UFID (student ID).
The program will output that student's top study matches with details about shared courses and preference matching.
## Data Format (data.csv)

Each row follows:

ufid,courses,environment,group_size,study_style
12345678,COP3502|MAC2311|PHY2048,quiet,small,focused
Where:

ufid is an 8-digit integer.
courses is a list separated by |.
environment can be: quiet, moderate, or lively.
group_size can be: small, medium, or large.
study_style can be: focused, casual, or interactive.

## Important Notes

Generated datasets overwrite data.csv by default.
Matching is based on both shared courses (high weight) and preference similarities (medium weight).
Jaccard similarity is used to evaluate how similar HashMatcher and GraphMatcher results are.

## Project Structure

├── main.cpp               # CLI interface and main driver

├── dataGenerator.cpp      # Random data generator

├── matcher.h               # IMatcher interface

├── hashMatcher.h           # HashMatcher class

├── graphMatcher.h          # GraphMatcher class

├── student.h               # Student data model

├── data.csv                # (Generated or provided dataset)
