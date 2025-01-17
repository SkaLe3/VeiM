import yaml
import sys
from pathlib import Path

def generate_target_file(output_path, target_name, target_configuration, target_type, project_file_path, launch_path):
    # Ensure the output directory exists
    output_path = Path(output_path)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    # Open the file to write the YAML data manually
    with open(output_path, "w") as file:
        # Write each key-value pair in the desired order using yaml.dump()
        yaml.dump({"TargetName": target_name}, file, default_flow_style=False)
        yaml.dump({"Configuration": target_configuration}, file, default_flow_style=False)
        yaml.dump({"TargetType": target_type}, file, default_flow_style=False)
        yaml.dump({"Project": str(project_file_path)}, file, default_flow_style=False)
        yaml.dump({"Launch": "$" + str(launch_path)}, file, default_flow_style=False)

    print(f"{target_name}.target file generated: {output_path}")

if __name__ == "__main__":
    # Ensure we have the correct number of arguments
    if len(sys.argv) != 7:
        print("Usage: python vmtarget.py <output_path> <target_name> <target_configuration> <target_type> <project_file_path> <launch_path>")
        sys.exit(1)

    # Parse arguments
    output_path = sys.argv[1]   # Path where the .target file will be saved
    target_name = sys.argv[2]   # The name of the target
    target_configuration = sys.argv[3]
    target_type = sys.argv[4]   # The type of the target (e.g., Game, Editor)
    project_file_path = sys.argv[5]  # Path to the project file
    launch_path = sys.argv[6]   # Path for launch (e.g., executable or script)

    print("launch path: " + launch_path)
    # Generate the target file
    generate_target_file(output_path, target_name, target_configuration, target_type, project_file_path, launch_path)
