from ROOT import gROOT, TFile, TTree, TTreeReader
import sys
import os

root_file_path = sys.argv[1]

print "Start extracting"
root_file = TFile.Open(root_file_path)



for k in root_file.GetListOfKeys():
    subdir = k.GetName();
    print "Extracting metadata %s" % (subdir)
    tree = root_file.Get(subdir).Get("metadata")
    f = open("%s_metadata.txt" % (subdir),"w+")
    for i in tree.GetUserInfo():
        f.write("key: %s, value: %s\n" % (i.GetName(), i.GetVal()))
    f.close()

    print "Extracting mask hits %s" % (subdir)
    tree = root_file.Get(subdir).Get("maskDeposits")
    f = open("%s_mask.txt" % (subdir),"w+")
    f.write("energy posX posY posZ eventId\n")
    for hitEntry in xrange(tree.GetEntries()):
        tree.GetEntry(hitEntry)
        energy = getattr(tree, 'energy')
        position = getattr(tree, 'position')
        id = getattr(tree, 'id')
        f.write("%f %f %f %f %d\n" %(energy, position.x(), position.y(), position.z(), id))
    f.close()

    print "Extracting detector hits %s" % (subdir)
    tree = root_file.Get(subdir).Get("deposits")
    f = open("%s_detector.txt" % (subdir),"w+")
    f.write("energy posX posY posZ eventId\n")
    for hitEntry in xrange(tree.GetEntries()):
        tree.GetEntry(hitEntry)
        energy = getattr(tree, 'energy')
        position = getattr(tree, 'position')
        id = getattr(tree, 'id')
        f.write("%f %f %f %f %d\n" %(energy, position.x(), position.y(), position.z(), id))
    f.close()

    print "Extracting source event %s" % (subdir)
    tree = root_file.Get(subdir).Get("source")
    f = open("%s_source.txt" % (subdir),"w+")
    f.write("energy posX posY posZ dirX dirY dirZ eventId\n")
    for hitEntry in xrange(tree.GetEntries()):
        tree.GetEntry(hitEntry)
        energy = getattr(tree, 'energy')
        position = getattr(tree, 'position')
        direction = getattr(tree, 'direction')
        id = getattr(tree, 'id')
        f.write("%f %f %f %f %f %f %f %d\n" %(energy, position.x(), position.y(), position.z(), direction.x(), direction.y(), direction.z(), id))
    f.close()

print "extracted to %s" % (root_file_path)
