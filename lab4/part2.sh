#!/bin/bash

# Path is set by first argument
path=$1

# If first argument (path) is not supplied, set it as current path (.)
if [ -z "$path" ]; then
	path="."
fi

# Process main files
echo "Main files:"

# Flag variable
hasMainFile=false

# Loop through all the files
for f in `find $path -type f`; do
    
	# Get the full file path
    full=`readlink -f $f`
    
	# If there is a line beginning with "int main"
    if (( `grep -c "^int main" $f` > 0 )); then
    
		# Set the flag
		hasMainFile=true
        #echo $f" contains a main function"
	
		# Count the number of occurrences of "printf" and "fprintf"
		FPRINTF_COUNT=`grep -c "fprintf" $f`
        PRINTF_COUNT=$(( `grep -c "printf" $f` - $FPRINTF_COUNT ))
	
		# Print out the full file name and number of occurrences of "printf" and "fprintf"
		printf "%s:%d,%d\n" $full $PRINTF_COUNT $FPRINTF_COUNT
		
    fi
    
done

# If flag is not set, no main files were found
if [ "$hasMainFile" = false ]; then
	echo "No main file"
fi

# Process Modules files
echo "Modules files:"

# Flag variable
hasModuleFile=false

# Loop through all the files
for f in `find $path -type f`; do
    
	# Get the full file path
    full=`readlink -f $f`
    
	# If there is a line beginning with "int init_module"
    if (( `grep -c "^int init_module" $f` > 0 )); then
    
		# Set the flag
		hasModuleFile=true
        #echo $full" contains an init_module function"
       
	    # Get the line numbers where "printk" occurs
        lineNumbers=`grep -n "printk" $f | cut -d : -f 1 | tr '\n' ','`
		# Remove the trailing comma
		lineNumbers=${lineNumbers%?}
        
		# Print out the full file name and line numbers where "printk" occurs
		printf "%s:%s\n" $full $lineNumbers

    fi
    
done

# If flag is not set, no module files were found
if [ "$hasModuleFile" = false ]; then
	echo "No module file"
fi


# Process other files
echo "Other files:"

# Flag variable
hasOtherFile=false

# Loop through all the files
for f in `find $path -type f`; do
    
	# Get the full file path
    full=`readlink -f $f`
    
	# If there are no lines beginning with "int main" or "int init_module"
    if (( `grep -c "^int main" $f` == 0 )); then
		if (( `grep -c "^int init_module" $f` == 0 )); then
    
			# Set the flag
			hasOtherFile=true
			#echo $f" contains no special functions"

			# Print out the full file name
			printf "%s\n" $full
			
		fi
    fi
    
done

# If flag is not set, no other files were found
if [ "$hasOtherFile" = false ]; then
	echo "No other file"
fi