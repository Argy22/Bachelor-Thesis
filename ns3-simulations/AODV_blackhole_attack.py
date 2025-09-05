import subprocess

NS3_DIR = "/home/user/ns-allinone-3.35/ns-3.35/"
SCRIPT_NAME = "AODV-Version"

# Ορισμός κακόβουλου κόμβου
blackhole_id=1 #default

def run_simulation(scenario=1, enable_attack=False, blackhole_id=blackhole_id, start_time=10.0, stop_time=50.0):
    
    
    # Κατασκευή εντολής για την εκτέλεση του ns-3
    command = [
        f'./waf',
        f'--run',
        f'{SCRIPT_NAME}',
        f'--command-template="%s --enableAttack={str(enable_attack).lower()} '
        f'--scenario={scenario} '
        f'--blackholeNodeId={blackhole_id} '
        f'--attackStartTime={start_time} '
        f'--attackStopTime={stop_time}"'
    ]
    
    # Μετατροπή της λίστας σε string για εκτέλεση
    full_command = " ".join(command)

    print("="*60)
    print(f"Executing command:\n{full_command}")
    print("="*60)
    
    try:
        # Εκτέλεση της εντολής από τον κατάλογο του ns-3
        subprocess.run(full_command, shell=True, check=True, cwd=NS3_DIR)
        print("\nSimulation finished successfully.\n")
    except subprocess.CalledProcessError as e:
        print(f"\nSimulation failed with error: {e}")
    except FileNotFoundError:
        print(f"Error: ns-3 directory '{NS3_DIR}' not found. Please check the path.\n")


if __name__ == "__main__":
    
    # Σενάριο 1: Εκτέλεση χωρίς επίθεση
    print(f"\n--- RUNNING SCENARIO 1: NO ATTACK ---")
    run_simulation(scenario=1)
    
    # Σενάριο 2: Εκτέλεση Blackhole επίθεσης
    print(f"\n--- RUNNING SCENARIO 2: BLACKHOLE ATTACK ---")
    run_simulation(scenario=2, 
                   enable_attack=True, blackhole_id=blackhole_id, 
                   start_time=10.0, stop_time=50.0)
    
    # Σενάριο 3: Εκτέλεση Blackhole και RERR flood επίθεσης
    print(f"\n--- RUNNING SCENARIO 3: BLACKHOLE ATTACK + RERR ---")
    run_simulation(scenario=3, 
                   enable_attack=True, blackhole_id=blackhole_id, 
                   start_time=10.0, stop_time=50.0)
    
    # Σενάριο 4: Εκτέλεση Blackhole και RREQ flood επίθεσης
    print(f"\n--- RUNNING SCENARIO 4: BLACKHOLE ATTACK + RREQ FLOOD ---")
    run_simulation(scenario=4, 
                   enable_attack=True, blackhole_id=blackhole_id, 
                   start_time=10.0, stop_time=50.0)

