if [ "$linux_run" == "yes" ]; then
    env_opt="env AAA= USER= a= b= ARCH=x86 ARCH=postk"
else
    env_opt="-e AAA -e USER= -e a -e b -e ARCH=x86 -e ARCH=postk"
fi
