@echo off
REM 
cd /d "%~dp0"
mkdir logs 2>nul

echo Running Standard Allocation Test...
REM 
..\memsim.exe < test_scenario_1.txt > logs\output_allocation.txt
echo Done. Output saved to logs\output_allocation.txt

echo Running Buddy System Test...
..\memsim.exe < test_buddy.txt > logs\output_buddy.txt
echo Done. Output saved to logs\output_buddy.txt

REM 
echo Running Virtual Memory ^& Cache Test...
..\memsim.exe < test_vm.txt > logs\output_vm.txt
echo Done. Output saved to logs\output_vm.txt

echo All tests completed.
pause
