import sys

if __name__ == "__main__":
  src_xml_file = sys.argv[1]
  compare_xml_file = sys.argv[2]

  src_file = open(src_xml_file, 'r')
  compare_file = open(compare_xml_file, 'r')

  for src_line in src_file:
      compare_line = compare_file.readline()
      if (src_line != compare_line):
          print("Lines do not match: {0} and {1}".format(
            src_line, compare_line))
