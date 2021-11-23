import uproot
from collections import namedtuple
import numpy as np
from tqdm import tqdm
from scipy.stats import multivariate_normal
import scipy.optimize as opt

SCORE = {"mse": 0, "uqi": 1, "both": 2}

# XY = namedtuple("XY", "x y")
GAUSS2D = namedtuple("gauss_params", "meanx meany sigmax sigmay cov ampl cut")


class Edges(namedtuple("XY", "x y")):
    """Class inherited from named tuple which represents
    edges of the histogram coordinates.

    Raises
    ------
    ValueError
        If trying to get binWidth when widths are different for X and Y
    """
    @property
    def x_binWidth(self):
        return 0.5*abs(self.x[1] - self.x[0])

    @property
    def y_binWidth(self):
        return 0.5*abs(self.y[1] - self.y[0])

    @property
    def binWidth(self):
        if self.x_binWidth == self.y_binWidth:
            return self.x_binWidth
        else:
            raise ValueError("Different widths for X and Y")

    @property
    def x_cent(self):
        return self.x[:-1] - sign(self.x[1] - self.x[0])*self.x_binWidth

    @property
    def y_cent(self):
        return self.y[:-1] - sign(self.y[1] - self.y[0])*self.y_binWidth


def sign(x):
    return (1, -1)[x > 0]


class Histogram(namedtuple('Histogram', ['vals', 'edges', 'name'])):

    vals: np.array
    edges: Edges
    name: str

    def __repr__(self):
        return "Histogram {}".format(self.name)


def get_histo(path, histo_names=["energyDeposits", "sourceHist"]):
    """Get histogram(s) from the .root file

    Args:
        path (str): Path to the root file
        histo_names (str or iterable of strings): Name(s) of histograms

    Returns:
        list[Histogram]: List of 'Histogram' objects
    """
    histo_names_it = [histo_names] if type(histo_names) == str else histo_names
    result = []
    with uproot.open(path) as file:
        for name in histo_names_it:
            vals, edgesx, edgesy = file[name].to_numpy()
            result.append(Histogram(vals[:, ::-1],
                                    Edges(edgesx, edgesy[::-1]),
                                    name))
    return result[0] if type(histo_names) == str else result


def get_hmat(path, norm=True):
    with uproot.open(path) as matr_file:
        Tmatr = matr_file["matrixH"]
        matrixH = np.array(Tmatr.member("fElements"))\
            .reshape(Tmatr.member("fNrows"), Tmatr.member("fNcols"))
    if norm:
        matrixH = matrixH/matrixH.sum(axis=0)  # normalization
    return matrixH


def get_source_edges(path):
    return get_histo(path, "sourceHist").edges


def ls(path):
    with uproot.open(path) as file:
        return file.keys()


def get(path, objname):
    with uproot.open(path) as file:
        return file[objname]


def normalize(x):
    if np.unique(x).shape[0] > 1:
        return (x - x.min())/(x.max() - x.min())
    else:
        return np.ones_like(x)


def mse_uqi(x, y, normx=False, normy=True):
    if normx:
        x = normalize(x)
    if normy:
        y = normalize(y)
    if x.shape != y.shape:
        raise ValueError("Shapes of arrays are different")
    # rmse = np.sqrt(((y - x)**2).mean())
    mse = ((y - x)**2).mean()
    cov = np.cov(x.flatten(), y.flatten())
    uqi = 4*x.mean()*y.mean()*cov[0, 1]/(cov[0, 0]+cov[1, 1])\
        / (x.mean()**2 + y.mean()**2)
    if uqi == 0:
        uqi = 1e-2
    return mse, 1/uqi, mse/uqi


def mse_uqi_set(x_set, y, normx=False, normy=True):
    if normy:
        y = normalize(y)
    mse = []
    uqi = []
    comb = []
    for x in x_set:
        mse_tmp, uqi_tmp, comb_tmp = mse_uqi(x, y, normy=False, normx=normx)
        mse.append(mse_tmp)
        uqi.append(uqi_tmp)
        comb.append(comb_tmp)
    return mse, uqi, comb


def reco_mlem(matr, image, niter, reco=None):
    if not reco:
        reco = [np.ones(matr.shape[-1])]
    for _ in tqdm(range(niter), desc="Reconstruction"):
        reco_tmp = reco[-1]*(matr.T @ (image/(matr @ reco[-1])))
        reco.append(normalize(reco_tmp))
    return reco


def reco_mlem_auto(matr, image, source,
                   reco=None, method="both", maxiter=2000):
    source_norm = normalize(source)
    if not reco:
        reco = [np.ones(matr.shape[-1])]
    score = mse_uqi_set(reco, source_norm, normy=False)[SCORE[method]]
    with tqdm(total=maxiter, desc="Reconstruction(autoiter)") as pbar:
        while len(reco) == 1 or score[-1] < score[-2]:
            reco_tmp = reco[-1]*(matr.T @ (image/(matr @ reco[-1])))
            reco.append(normalize(reco_tmp))
            score.append(mse_uqi(reco[-1], source_norm)[SCORE[method]])
            pbar.update(1)
        # pbar.update(maxiter - len(reco) + 1)
    print(f"{len(reco)-2} iterations was performed")
    print(f"Best score: {round(score[-2],5)}")
    return reco[:-1]


def reco_1d_from_file(filename, hmat2, niter=100):
    """make reconstruction 1D from file"""
    sim = get_histo(filename, "energyDeposits")
    sim_row = sim.vals.sum(axis=1).reshape(-1)
    im = reco_mlem(hmat2, sim_row, niter)
    return im


def get_hymped_hmat(path, norm=True):
    with uproot.open(path) as matr_file:
        Tmatr2 = matr_file["matrixH0"]
        matrixH0 = np.array(Tmatr2.member("fElements"))\
            .reshape(Tmatr2.member("fNrows"), Tmatr2.member("fNcols"))

        Tmatr2 = matr_file["matrixH1"]
        matrixH1 = np.array(Tmatr2.member("fElements"))\
            .reshape(Tmatr2.member("fNrows"), Tmatr2.member("fNcols"))

        Tmatr2 = matr_file["matrixH2"]
        matrixH2 = np.array(Tmatr2.member("fElements"))\
            .reshape(Tmatr2.member("fNrows"), Tmatr2.member("fNcols"))
    matrix_all = np.vstack((matrixH0, matrixH1, matrixH2))
    if norm:
        matrix_all /= matrix_all.sum(axis=0)
    return matrix_all


def get_hypmed_sim_row(path):
    simdata = get_histo(
        path, [f"energyDepositsLayer{i}" for i in range(3)])
    return np.hstack([sim.vals.flatten() for sim in simdata])


def get_sim_row(path):
    simdata = get_histo(path, "energyDeposits")
    return simdata[0].vals.flatten()


def Gaussian_2D(xdata_tuple,
                meanx, meany,
                sigmax, sigmay, cov,
                amplitude, cut):
    """Probability density function for the 2D multivariate normal distribution
    with custom amplitude and shift along Y.
    This function is used for fitting.

    Parameters
    ----------
    xdata_tuple : tuple(numpy.array, numpy.array)
        Tuple of the X and Y coordinates
    meanx : float
            Location parameter for X axis
    meany : float
            Location parameter for Y axis
    sigmax : float
             Variance for X axis
    sigmay : float
             Variance for Y axis
    cov : float
          cov(X, Y)
    amplitude : float
    cut : float

    Returns
    -------
    numpy.array
        Flattened array of the function values
    """
    xdata_tuple = np.meshgrid(*xdata_tuple)
    cov_matrix = [[sigmax, cov], [cov, sigmay]]
    return cut + amplitude*multivariate_normal.pdf(np.dstack(xdata_tuple),
                                                   [meanx, meany],
                                                   cov_matrix).ravel()


def fit_2d(x, y, data):
    """Fir the function values with 2D Gaussian

    Parameters
    ----------
    x : numpy.array
    y : numpy.array
    data : numpy.array
        2D numpy array with function values

    Returns
    -------
    GAUSS2D
        fitting parameters
    """
    meanx = x[np.argmax(data.sum(axis=0))]
    meany = y[np.argmax(data.sum(axis=1))]
    popt, _ = opt.curve_fit(
        Gaussian_2D, (x, y), data.ravel(), (meanx, meany, 1, 1, 0.0, 1, 0))
    return GAUSS2D(*popt)


def reco_mlem_last(matr, image, niter, reco=None):
    """Reconstruction which returns only the last image
    """
    if not reco:
        reco = np.ones(matr.shape[-1])
    else:
        reco = reco[-1]
    for _ in range(niter):
        reco_tmp = reco*(matr.T @ (image/(matr @ reco)))
        reco = reco_tmp
    return reco
