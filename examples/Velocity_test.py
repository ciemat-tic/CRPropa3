#!/usr/bin/env python3
import os
import sys
try:
    import numpy as np
except ModuleNotFoundError:
    print("Falta numpy en este python. Ejecuta primero ./install.sh y despues source activate.sh.", file=sys.stderr)
    raise SystemExit(1)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_DIR_NAME = "velocity_test_outputs"
NEVENTS = 1
PY_VER = f"python{sys.version_info.major}.{sys.version_info.minor}"


def find_crpropa_home():
    env_home = os.environ.get("CRPROPA_HOME")
    if env_home and os.path.isdir(env_home):
        return os.path.abspath(env_home)

    candidates = [
        BASE_DIR,
        os.path.abspath(os.path.join(BASE_DIR, "..", "CRPropa3")),
        os.path.abspath(os.path.join(BASE_DIR, "..", "..", "..")),
    ]

    for path in candidates:
        if os.path.isfile(os.path.join(path, "CMakeLists.txt")):
            return path
        if os.path.isdir(os.path.join(path, "lib")) and os.path.isdir(os.path.join(path, "share")):
            return path

    return BASE_DIR


CRPROPA_HOME = find_crpropa_home()
OUTDIR = os.environ.get(
    "CRPROPA_TEST_OUTDIR",
    os.path.join(CRPROPA_HOME, OUTPUT_DIR_NAME),
)

def add_path(path):
    if path and os.path.isdir(path) and path not in sys.path:
        sys.path.insert(0, path)

os.environ.setdefault("CRPROPA_HOME", CRPROPA_HOME)

data_path = os.path.join(CRPROPA_HOME, "share", "crpropa")
if os.path.isdir(data_path):
    os.environ.setdefault("CRPROPA_DATA_PATH", data_path)

add_path(os.path.join(CRPROPA_HOME, "lib", PY_VER, "site-packages"))
add_path(os.path.join(CRPROPA_HOME, "build"))

try:
    from crpropa import *
except ModuleNotFoundError:
    print("No se encuentra el modulo crpropa. Ejecuta primero ./install.sh y despues source activate.sh.", file=sys.stderr)
    raise SystemExit(1)

os.makedirs(OUTDIR, exist_ok=True)

# Energías cinéticas del electrón [eV]
energies_eV = [1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e12]

dSrc = 1.0 * Mpc

def enable_if_exists(out, colname):
    if hasattr(Output, colname):
        out.enable(getattr(Output, colname))
    else:
        print(f"[WARNING] Output.{colname} not found")

def configure_output(out):
    out.disableAll()

    for col in [
        "TrajectoryLengthColumn",
        "TimeColumn",
        "CurrentIdColumn",
        "CurrentEnergyColumn",
        "CurrentPositionColumn",
        "CurrentDirectionColumn",
    ]:
        enable_if_exists(out, col)

    out.setEnergyScale(eV)
    out.setLengthScale(Mpc)

for E_eV in energies_eV:
    tag = f"E{E_eV:.0e}"

    source = Source()
    source.add(SourcePosition(Vector3d(0, 0, 0)))
    source.add(SourceDirection(Vector3d(1, 0, 0)))
    source.add(SourceParticleType(11))  # electron
    source.add(SourceEnergy(E_eV * eV))

    event_file = os.path.join(OUTDIR, f"event_{tag}.txt")
    traj_file = os.path.join(OUTDIR, f"traj_{tag}.txt")

    event_out = TextOutput(event_file, Output.Event3D)
    traj_out = TextOutput(traj_file, Output.Trajectory3D)

    configure_output(event_out)
    configure_output(traj_out)

    observer = Observer()
    observer.add(ObserverSurface(Sphere(Vector3d(dSrc, 0, 0), 1 * kpc)))
    observer.onDetection(event_out)

    sim = ModuleList()

    # Sin interacciones, sin campo magnético, sin redshift.
    sim.add(SimplePropagation(1 * pc, 10 * kpc))
    sim.add(traj_out)
    sim.add(observer)
    sim.add(MaximumTrajectoryLength(2 * dSrc))

    sim.setShowProgress(False)

    print(f"Running velocity test: Ekin = {E_eV:.3e} eV")
    sim.run(source, NEVENTS, True)

    event_out.close()
    traj_out.close()

print(f"Done. Outputs in {OUTDIR}")

#!/usr/bin/env python3
import os
import glob
try:
    import numpy as np
except ModuleNotFoundError:
    print("Falta numpy en este python. Ejecuta primero ./install.sh y despues source activate.sh.", file=sys.stderr)
    raise SystemExit(1)

try:
    import matplotlib.pyplot as plt
except ModuleNotFoundError:
    plt = None

OUTDIR = os.environ.get(
    "CRPROPA_TEST_OUTDIR",
    os.path.join(CRPROPA_HOME, OUTPUT_DIR_NAME),
)

Mpc_SI = 3.0856775814913673e22
Myr_SI = 1e6 * 365.25 * 24 * 3600
c_SI = 2.99792458e8

m_e_c2_eV = 510998.950  # electron rest energy [eV]

def beta_from_kinetic_energy(Ekin_eV):
    gamma = 1.0 + Ekin_eV / m_e_c2_eV
    return np.sqrt(1.0 - 1.0 / gamma**2)

summary = []

for fname in sorted(glob.glob(os.path.join(OUTDIR, "event_E*.txt"))):
    data = np.loadtxt(fname, comments="#")

    if data.ndim == 1:
        data = data.reshape(1, -1)

    base = os.path.basename(fname)
    E_eV = float(base.split("event_E")[1].split(".txt")[0])

    D_Mpc = data[0, 0]
    t_Myr = data[0, 1]

    beta_output = (D_Mpc * Mpc_SI) / (t_Myr * Myr_SI * c_SI)
    beta_expected = beta_from_kinetic_energy(E_eV)

    summary.append([E_eV, D_Mpc, t_Myr, beta_output, beta_expected])

summary = np.array(summary)
summary = summary[np.argsort(summary[:, 0])]

np.savetxt(
    os.path.join(OUTDIR, "velocity_summary.txt"),
    summary,
    header="Ekin_eV D_Mpc time_Myr beta_output beta_expected"
)

print("E_eV        D[Mpc]       time[Myr]     beta_output     beta_expected")
for row in summary:
    print(f"{row[0]:.3e}  {row[1]:.6e}  {row[2]:.6e}  {row[3]:.8e}  {row[4]:.8e}")

if plt is not None:
    plt.figure(figsize=(6, 4))
    plt.semilogx(summary[:, 0], summary[:, 3], marker="o", label="CRPropa output")
    plt.semilogx(summary[:, 0], summary[:, 4], marker="s", ls="--", label="Expected beta")
    plt.axhline(1.0, ls=":")
    plt.xlabel("Kinetic energy [eV]")
    plt.ylabel(r"$D/(ct)$")
    plt.legend()
    plt.grid(True, which="both")
    plt.tight_layout()
    plt.savefig(os.path.join(OUTDIR, "velocity_ratio.pdf"))
    plt.show()
