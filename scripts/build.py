#!/bin/python3

import os
import sys
import shutil
import argparse
import subprocess
import multiprocessing


usage = """
Usage

    CLI HSharp VE Engine builder is used to set up build environment, check all 
    dependencies and run CMake build/configure routines.
    -b --build          run CMake build routine. Requires configured environment.
    -c --configure      configure CMake and check environment utils.
    -f --force          remove build directory before executing any CMake commands.
       --dir            specify build directory
       --config CONFIG  run for specified config. (Release, Debug) Default - only Debug.
                        You might invoke this multiple times.
       --link           create symlink in project root to Debug compile commands.

"""


def configure(args: argparse.Namespace) -> None:
    requires = ['cmake']
    print('running configure. checking requirements: ' + ', '.join(requires))
    for utility in requires:
        utility_check(utility)

    directory, configs = get_common(args)

    force = getattr(args, 'force', None)
    if force and os.path.exists(directory) and os.path.isdir(directory):
        confirm = input(f'force-removing directory: {directory}. Are you sure? (N/y) ')
        if not confirm:
            confirm = 'n'
        if confirm != 'y':
            sys.exit(1)
        shutil.rmtree(directory)
        os.mkdir(directory)

    print('configuring for CMake build configs: ' + ', '.join(configs))

    cmake_args = []
    print('running cmake with args: ' + ', '.join(cmake_args))
    for config in configs:
        print(f'running configure command for config {config}...')
        subprocess.run([
                'cmake', 
                f'-B{directory}/{config}', 
                f'-S{os.curdir}', 
                f'-DCMAKE_BUILD_TYPE={config}', 
                *cmake_args
            ], encoding='UTF-8', stderr=subprocess.STDOUT
        )


def build(args: argparse.Namespace) -> None:
    requires = ['cmake', 'gcc']
    print('running build. checking requirements: ' + ', '.join(requires))
    for utility in requires:
        utility_check(utility)
    
    directory, configs = get_common(args)

    print(f'using directory: {directory}')

    if not os.path.exists(directory):
        print('build directory not found.')
        sys.exit(1)
    
    for config in configs:
        print(f'running build command for config {config}...')
        subprocess.run([
                'cmake', 
                '--build', 
                f'{directory}/{config}', 
                '-j', 
                str(multiprocessing.cpu_count())
            ], encoding='UTF-8', stderr=subprocess.STDOUT
        )


def cmake_var_decorator(var: str, value: str, type: str = 'STRING') -> str:
    return f'-D{var.upper()}:{type}={value}'


def utility_check(name: str) -> None:
    print(f'checking utility: {name}, running $PATH check...', end=' ')
    locations = subprocess.run(['whereis', name], stdout=subprocess.PIPE, encoding='UTF-8').stdout.split()[1:]
    if not locations:
        print('failed')
        sys.exit(1)
    locations_string = 'found utility in: ' + ', '.join(locations)
    print('ok', locations_string, sep='\n', end='\n')


def link_compile_commands(path: str) -> None:
    if os.path.exists('compile_commands.json'):
        print('compile commands file exists, skipping link creation')
    os.symlink(path, 'compile_commands.json')
    print('symlink created')


def parse_cli_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        add_help=False
    )
    parser.add_argument('-h', '--help', action='store_true', dest='help')
    parser.add_argument('-b', '--build', action='store_true', dest='build')
    parser.add_argument('-c', '--configure', action='store_true', dest='configure')
    parser.add_argument('-f', '--force', action='store_true', dest='force')
    parser.add_argument('--dir', action='store', dest='directory')
    parser.add_argument('--config', action='append', dest='config')
    parser.add_argument('--link', action='store_true', dest='link')
    return parser.parse_args()


def get_common(args: argparse.Namespace) -> list[str | list[str]]:
    directory = getattr(args, 'directory', None)
    if not directory:
        directory = 'build'

    configs = getattr(args, 'config', None)
    if configs is None:
        configs = ['Debug']

    return directory, configs


if __name__ == '__main__':
    args = parse_cli_args()

    if getattr(args, 'help'):
        print(usage)
        sys.exit(0)

    if getattr(args, 'configure'):
        print('invoking configure routine.')
        configure(args)

    if getattr(args, 'build'):
        print('invoking build routine.')
        build(args)

    if getattr(args, 'link'):
        directory, _ = get_common(args)
        link_compile_commands(directory + '/Debug/compile_commands.json')

    print('finished.')