from ROOT import gROOT, TFile, TTree, TTreeReader
import sys
import os

root_file_path = sys.argv[1]
output_postfix = sys.argv[2]

print "Start extracting"
root_file = TFile.Open(root_file_path)


f = open("events_stats_%s.txt" % (output_postfix),"w+")

tree_meta = root_file.Get(root_file.GetListOfKeys()[0].GetName()).Get("metadata")
for i in tree_meta.GetUserInfo():
    f.write("%s " % (i.GetName()))

f.write("nMaskDetector nMaskOnly nDetectorOnly nMaskMult nDetMult\n")

for k in root_file.GetListOfKeys():
    subdir = k.GetName();
    print "Processing event from %s" % (subdir)

    tree_meta = root_file.Get(subdir).Get("metadata")
    tree_mask = root_file.Get(subdir).Get("maskDeposits")
    tree_src = root_file.Get(subdir).Get("source")
    tree_det = root_file.Get(subdir).Get("deposits")

    n_md_events = 0;
    n_monly_events = 0;
    n_donly_events = 0;
    n_m_multi_events = 0;
    n_d_multi_events = 0;

    detector_entry = 0;
    mask_entry = 0;
    
    for src_entry in xrange(tree_src.GetEntries()):
        tree_src.GetEntry(src_entry)
        tree_mask.GetEntry(mask_entry)
        tree_det.GetEntry(detector_entry)

        event_id = getattr(tree_src, "id")
        current_mask_event = getattr(tree_mask, "id")
        current_detector_event = getattr(tree_det, "id")

        if current_mask_event > event_id and current_detector_event > event_id:
            continue

        event_in_mask = event_id == current_mask_event
        event_in_detector = event_id == current_detector_event

        mask_counter = 0
        while event_id == current_mask_event and mask_entry < tree_mask.GetEntries():
            mask_counter += 1
            mask_entry += 1
            tree_mask.GetEntry(mask_entry)
            current_mask_event = getattr(tree_mask, "id")

        detector_counter = 0
        while event_id == current_detector_event and detector_entry < tree_det.GetEntries():
            detector_counter += 1
            detector_entry += 1
            tree_det.GetEntry(detector_entry)
            current_detector_event = getattr(tree_det, "id")



        if event_in_mask and event_in_detector:
            n_md_events += 1

        if event_in_mask and not event_in_detector:
            n_monly_events +=1

        if event_in_detector and not event_in_mask:
            n_donly_events += 1

        if mask_counter > 1:
            n_m_multi_events += 1

        if detector_counter > 1:
            n_d_multi_events += 1
         
    
    for i in tree_meta.GetUserInfo():
        f.write("%s " % (i.GetVal()))
    f.write("%d %d %d %d %d\n" % (n_md_events, n_monly_events, n_donly_events, n_m_multi_events, n_d_multi_events))
 
    print ("Result: nMaskDetector=%d nMaskOnly=%d nDetectorOnly=%d nMaskMult=%d nDetMult=%d\n" % (n_md_events, n_monly_events, n_donly_events, n_m_multi_events, n_d_multi_events))

f.close()
