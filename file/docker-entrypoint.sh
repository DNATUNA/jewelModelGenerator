echo "RUN jewel"
cd ~/jewelModelGenerator

exec ./jewel ./file/"$fileNameWithExtension" ./file/"$fileName".stl
