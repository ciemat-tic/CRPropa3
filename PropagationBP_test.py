#!/usr/bin/env python3
import os
import sys
try:
    import numpy as np
except ModuleNotFoundError:
    print("Falta numpy en este python. Ejecuta primero ./install.sh y despues source activate.sh.", file=sys.stderr)
    raise SystemExit(1)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_DIR_NAME = "bfield_larmor_outputs"
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

energies_eV = [1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e12]

B_NG = 1.0
B0 = B_NG * nG
bfield = UniformMagneticField(Vector3d(0, 0, B0))

eV_J = 1.602176634e-19
c_SI = 2.99792458e8
m_e_kg = 9.1093837015e-31
q_e_C = 1.602176634e-19
kpc_m = 3.0856775814913673e19
B_T = B_NG * 1e-13

def larmor_radius_kpc(Ekin_eV):
    T = Ekin_eV * eV_J
    mc2 = m_e_kg * c_SI**2
    pc = np.sqrt(T * (T + 2.0 * mc2))
    p = pc / c_SI
    r_m = p / (q_e_C * B_T)
    return r_m / kpc_m

def enable_if_exists(out, colname):
    if hasattr(Output, colname):
        out.enable(getattr(Output, colname))

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
    out.setLengthScale(kpc)

for E_eV in energies_eV:
    tag = f"E{E_eV:.0e}"
    r_th = larmor_radius_kpc(E_eV)
    step = (r_th / 500.0) * kpc

    source = Source()
    source.add(SourcePosition(Vector3d(0, 0, 0)))
    source.add(SourceDirection(Vector3d(1, 0, 0)))
    source.add(SourceParticleType(11))  # electron
    source.add(SourceEnergy(E_eV * eV))

    traj_file = os.path.join(OUTDIR, f"traj_larmor_{tag}.txt")
    traj_out = TextOutput(traj_file, Output.Trajectory3D)
    configure_output(traj_out)

    sim = ModuleList()

    # Campo B = Bz, electrón inicial en x, movimiento en xy.
    # El paso se escala con el radio de Larmor para mantener precisión
    # homogénea en todo el rango de energías.
    sim.add(PropagationBP(bfield, step))
    sim.add(traj_out)

    # Se integra solo un par de radios de giro, suficiente para ajustar el círculo.
    sim.add(MaximumTrajectoryLength(2.0 * r_th * kpc))

    sim.setShowProgress(False)

    print(
        f"Running Larmor test: Ekin = {E_eV:.3e} eV, B = {B_NG} nG, "
        f"step = {step / kpc:.3e} kpc"
    )
    sim.run(source, NEVENTS, True)

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
B_NG = 1.0

eV_J = 1.602176634e-19
c_SI = 2.99792458e8
m_e_kg = 9.1093837015e-31
q_e_C = 1.602176634e-19
kpc_m = 3.0856775814913673e19

B_T = B_NG * 1e-13

def larmor_radius_kpc(Ekin_eV):
    T = Ekin_eV * eV_J
    mc2 = m_e_kg * c_SI**2

    # pc = sqrt(T^2 + 2 T mc^2)
    pc = np.sqrt(T * (T + 2.0 * mc2))
    p = pc / c_SI

    r_m = p / (q_e_C * B_T)
    return r_m / kpc_m

def fit_circle(x, y):
    M = np.column_stack([x, y, np.ones_like(x)])
    rhs = -(x**2 + y**2)

    A, B, C = np.linalg.lstsq(M, rhs, rcond=None)[0]

    xc = -A / 2.0
    yc = -B / 2.0
    r = np.sqrt((A**2 + B**2) / 4.0 - C)

    return xc, yc, r

summary = []

for fname in sorted(glob.glob(os.path.join(OUTDIR, "traj_larmor_E*.txt"))):
    data = np.loadtxt(fname, comments="#")

    if data.ndim == 1:
        data = data.reshape(1, -1)

    base = os.path.basename(fname)
    E_eV = float(base.split("traj_larmor_E")[1].split(".txt")[0])

    # Columnas esperadas:
    # D, time, ID, E, X, Y, Z, Px, Py, Pz
    x = data[:, 4]
    y = data[:, 5]

    if len(x) < 10:
        print(f"Not enough points in {fname}")
        continue

    xc, yc, r_fit = fit_circle(x, y)
    r_th = larmor_radius_kpc(E_eV)

    rel_err = abs(r_fit - r_th) / r_th

    summary.append([E_eV, r_th, r_fit, rel_err, xc, yc])

summary = np.array(summary)
summary = summary[np.argsort(summary[:, 0])]

np.savetxt(
    os.path.join(OUTDIR, "larmor_summary.txt"),
    summary,
    header="Ekin_eV r_theory_kpc r_fit_kpc relative_error x_center_kpc y_center_kpc"
)

print("E_eV        r_theory[kpc]   r_fit[kpc]      rel_error")
for row in summary:
    print(f"{row[0]:.3e}  {row[1]:.6e}     {row[2]:.6e}   {row[3]:.6e}")

if plt is not None:
    plt.figure(figsize=(6, 4))
    plt.loglog(summary[:, 0], summary[:, 1], marker="o", label="Theory")
    plt.loglog(summary[:, 0], summary[:, 2], marker="s", ls="--", label="CRPropa fit")
    plt.xlabel("Kinetic energy [eV]")
    plt.ylabel(r"$r_L$ [kpc]")
    plt.legend()
    #plt.grid(True, which="both")
    plt.tight_layout()
    plt.savefig(os.path.join(OUTDIR, "larmor_radius_comparison.pdf"))
    plt.show()

    plt.figure(figsize=(6, 4))
    plt.semilogx(summary[:, 0], summary[:, 3], marker="o")
    plt.xlabel("Kinetic energy [eV]")
    plt.ylabel("Relative error")
    #plt.grid(True, which="both")
    plt.tight_layout()
    plt.savefig(os.path.join(OUTDIR, "larmor_relative_error.pdf"))
    plt.show()
