import argparse
from os.path import curdir
import subprocess
import os

def get_command_path(command):
    """Get the full path of the specified command using 'which'."""
    try:
        return subprocess.check_output(['which', command], text=True).strip()
    except subprocess.CalledProcessError:
        print(f"Error: {command} not found.")
        exit(1)

def main():
    parser = argparse.ArgumentParser(
        prog="build",
        description='Utility script to build the project',
    )
    parser.add_argument('-c', '--compiler-name', choices=['gcc', 'clang'], required=True,
                        help="the compiler you want to use, gcc or clang")
    args = parser.parse_args()

    # Determine which compilers to use based on the argument
    if args.compiler_name == 'gcc':
        cxx_compiler = 'g++'
        c_compiler = 'gcc'
    else:
        cxx_compiler = 'clang++'
        c_compiler = 'clang'
    
    # Get the compiler paths
    cxx_compiler_path = get_command_path(cxx_compiler)
    c_compiler_path = get_command_path(c_compiler)
    make_program_path = get_command_path('ninja')  # Get the path for ninja

    vcpkg_root = os.getenv('VCPKG_ROOT')
    if vcpkg_root is None:
        print("VCPKG_ROOT is not set, either set it or install vcpkg at: https://vcpkg.io/en/")
        exit(1)

    if(not os.path.exists(f'{os.curdir}/build')):
        os.makedirs(f'{os.curdir}/build')
        

    cmake_command = [
        'cmake',
        '--preset=vcpkg',
        '-D', f'CMAKE_MAKE_PROGRAM={make_program_path}',
        '-D', f'CMAKE_CXX_COMPILER={cxx_compiler_path}',
        '-D', f'CMAKE_C_COMPILER={c_compiler_path}',
        '-D', f'CMAKE_TOOLCHAIN_FILE={vcpkg_root}/scripts/buildsystems/vcpkg.cmake'
    ]

    # Execute the cmake command
    try:
        subprocess.run(cmake_command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error during CMake execution: {e}")
        exit(1)

    os.chdir(f"{os.curdir}/build");
    try: 
        subprocess.run("ninja", check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error during ninja execution: {e}")
        exit(1)




if __name__ == "__main__":
    main()

