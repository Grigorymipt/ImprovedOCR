pdfdir=$1
ls $pdfdir | grep .pdf | while read file 
do
  pdftoppm "pdf/$file" "photos/${file%.pdf}" -png
  echo "Presentation file $file is converted"
done
mkdir photos
g++ hought.cpp  -o main -I /usr/include/opencv4 -L /usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_objdetect
ls photos | grep .png | while read file
do 
  ./main $file photos
  echo "opencv script executed for $file"  
done
mkdir cropped
for dir in cropped/* 
do
  for file in $dir/*
  do
    tesseract "$file" - -l eng > "${file}.py"
    echo "text was recognized in $file"
    python3 askLlama.py "${file}.py" > "${file}-correctedbyLlama.py"
    rm "${file}.py"
  done
done
