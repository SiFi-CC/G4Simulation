import uproot
from collections import namedtuple
import numpy as np
from tqdm import tqdm

SCORE = {"mse": 0, "uqi": 1, "both": 2}


class Histogram(namedtuple('Histogram', ['vals', 'edges', 'name'])):

    vals: np.array
    edges: np.array
    name: str

    def __repr__(self):
        return "Histogram {}".format(self.name)


def get_histo(path, histo_names):
    """[summary]

    Args:
        path (str): Path to the root file
        histo_names (str or iterable of strings): Names of histograms

    Returns:
        list[Histogram]: List of 'Histogram' objects
    """
    if type(histo_names) == str:
        histo_names = [histo_names]
    result = []
    with uproot.open(path) as file:
        for name in histo_names:
            vals, edges, _ = file[name].to_numpy()
            result.append(Histogram(vals[:, ::-1], edges, name))
    return result


def get_hmat(path, norm=True):
    with uproot.open(path) as matr_file:
        Tmatr = matr_file["matrixH"]
    matrixH = np.array(Tmatr.member("fElements"))\
        .reshape(Tmatr.member("fNrows"), Tmatr.member("fNcols"))
    if norm:
        matrixH = matrixH/matrixH.sum(axis=0)  # normalization
    return matrixH


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
        uqi = 1e-8
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


def reco_mlem(matr, image, niter, reco=None, source=None):
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
