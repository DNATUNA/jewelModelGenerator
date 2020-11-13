echo "RUN jewel"
cd ~/jewelModelGenerator

export $(cat ./file/.env | xargs)

exec ./jewel ./file/"$fileNameWithExtension" ./file/"$fileName".stl
