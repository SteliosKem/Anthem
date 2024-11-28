import sys
import os
import subprocess

def read_and_replace(filename, word, replacement):
    with open(filename, 'r') as file:
        filedata = file.read()

    filedata = filedata.replace(word, replacement)

    with open(filename, 'w') as file:
        file.write(filedata)

if __name__ == '__main__':
    if len(sys.argv) == 0:
        print("Usage: [Platform] [ApplicationName] [LibraryName]")
        print("[Platform]: windows, macos, linux")
    else:
        platform = sys.argv[1]
        app_name = sys.argv[2]
        lib_name = sys.argv[3]

        # Replace Application
        os.chdir('..')
        read_and_replace("build.lua", "Application", app_name);
        read_and_replace("Application/build-app.lua", "Application", app_name);

        # Replace Library
        read_and_replace("build.lua", "Library", lib_name);
        read_and_replace("Library/build-lib.lua", "Library", lib_name);

        os.rename("Application", app_name)
        os.rename("Library", lib_name)

        if platform == 'windows':
            FNULL = open(os.devnull, 'w')
            args = "vendor/premake/Windows/premake5.exe --file=build.lua vs2022"
            subprocess.call(args, stdout=FNULL, stderr=FNULL, shell=False)