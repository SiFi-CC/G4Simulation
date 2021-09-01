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
    return (x - x.min())/(x.max() - x.min())


def mse_uqi(x, y, normx=True, normy=True):
    if normx:
        x = normalize(x)
    if normy:
        y = normalize(y)
    if x.shape != y.shape:
        raise ValueError("Shapes of arrays are different")
    mse = np.sqrt(((y - x)**2).mean())
    # mse = ((y - x)**2).sum()/x.shape[-1]**2
    cov = np.cov(x.flatten(), y.flatten())
    uqi = 4*x.mean()*y.mean()*cov[0, 1]/(cov[0, 0]+cov[1, 1])\
        / (x.mean()**2 + y.mean()**2)
    return mse, uqi, mse/uqi


def mse_uqi_set(x_set, y, normx=True, normy=True):
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
        reco.append(reco_tmp)
    return reco


def recoo_mlem_auto(matr, image, source, reco=None, method="both"):
    source_norm = normalize(source)
    if not reco:
        reco = [np.ones(matr.shape[-1])]
    if len(reco) > 1:
        score = [mse_uqi(reco[-2], source_norm, normy=False)[SCORE[method]],
                 mse_uqi(reco[-1], source_norm, normy=False)[SCORE[method]]]
    else:
        score = [mse_uqi(reco[-1], source_norm, normy=False)[SCORE[method]]]
    while len[reco] == 0 or score[-1] > score[-2]:
        reco_tmp = reco[-1]*(matr.T @ (image/(matr @ reco[-1])))
        reco.append(normalize(reco_tmp))
        score.append(mse_uqi(reco[-1], source_norm)[SCORE[method]])
    return reco, score


# class MLEM():

#     def __init__(self, simdata, matrixH, max_iter=2000,
#                  source=True):
#         self.max_iter = max_iter
#         self.system_matrix = get_hmat(matrixH)
#         if source:
#             self.simdata, self.source =\
#                 get_histo(simdata, ["energyDeposits", "sourceHist"])
#         else:
#             self.simdata = get_histo(simdata, "energyDeposits")
#             # TODO test dimensions correspondance
#         self.source = normalize(self.source)
#         self.image = [np.ones(self.system_matrix.shape[-1])]
#         self.current_iter = 0
#         self.mse = []
#         self.uqi = []

#     def reco(self, n_iter, score="both"):
#         if n_iter > 0:
#             self.image = reco_mlem(self.system_matrix,
#                                    self.simdata.vals.flatten(),
#                                    self.n_iter,
#                                    reco=self.image)
#         else:

#         self.current_iter = len(self.image)
