import pydicom
import numpy as np
import os

def read_dicom(file_path, fileKind):
    study_instance_uid = None
    series_instance_uid = None
    image_orientation = None
    pixel_spacing = None
    row_spacing = None
    column_spacing = None
    slice_thickness = None
    num_frames = 0


    if fileKind == "file":
        ds = pydicom.dcmread(file_path, force=True)

        study_instance_uid = getattr(ds, "StudyInstanceUID", None)
        series_instance_uid = getattr(ds, "SeriesInstanceUID", None)
        image_orientation = getattr(ds, "ImageOrientationPatient", None)

        pixel_spacing = getattr(ds, "PixelSpacing", None)  # A list [row_spacing, column_spacing]
        row_spacing, column_spacing = (pixel_spacing if pixel_spacing else (None, None))
        slice_thickness = getattr(ds, "SliceThickness", None)

        if row_spacing == None:
            row_spacing = 0

        if column_spacing == None:
            column_spacing = 0

        if slice_thickness == None:
            slice_thickness = 0

        if image_orientation == None:
            image_orientation = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]


        num_frames = 1

        frameArr = []
        if hasattr(ds, 'NumberOfFrames'):
            num_frames = int(ds.NumberOfFrames)
            frameArr = [ds.pixel_array[i] for i in range(num_frames)]
            if hasattr(ds, 'PhotometricInterpretation'):
                if ds.PhotometricInterpretation == "YBR_FULL":
                    for i in range(num_frames):
                        ds.pixel_array[i] = convert_color_space(ds.pixel_array[i], "YBR_FULL", "RGB")
                        frameArr.append(ds.pixel_array[i])
            else:
                for i in range(num_frames):
                    frameArr.append(ds.pixel_array[i])
        else:
            if hasattr(ds, 'PhotometricInterpretation'):
                if ds.PhotometricInterpretation == "YBR_FULL":
                    ds.pixel_array = convert_color_space(ds.pixel_array, "YBR_FULL", "RGB")
            frameArr.append(ds.pixel_array)

        framesNpArray = np.array(frameArr)


        return framesNpArray, study_instance_uid, series_instance_uid, image_orientation, row_spacing, column_spacing, slice_thickness, num_frames



    elif fileKind == "dir":
        i = 0
        frameArr = []
        for file in os.listdir(file_path):
            full_file_path = os.path.join(file_path, file)
            ds = pydicom.dcmread(full_file_path, force=True)

            if not 'PixelData' in ds:
                continue

            if 'TransferSyntaxUID' not in ds.file_meta:
                ds.file_meta.TransferSyntaxUID = pydicom.uid.ImplicitVRLittleEndian

            if i == 0:
                study_instance_uid = getattr(ds, "StudyInstanceUID", None)
                series_instance_uid = getattr(ds, "SeriesInstanceUID", None)
                image_orientation = getattr(ds, "ImageOrientationPatient", None)

                pixel_spacing = getattr(ds, "PixelSpacing", None)  # A list [row_spacing, column_spacing]
                row_spacing, column_spacing = (pixel_spacing if pixel_spacing else (None, None))
                slice_thickness = getattr(ds, "SliceThickness", None)

                if row_spacing == None:
                    row_spacing = 0

                if column_spacing == None:
                    column_spacing = 0

                if slice_thickness == None:
                    slice_thickness = 0

                if image_orientation == None:
                    image_orientation = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                i += 1


            if hasattr(ds, 'NumberOfFrames'):
                num_frames += int(ds.NumberOfFrames)
                for i in range(num_frames):
                    frameArr.append(ds.pixel_array[i])
                if hasattr(ds, 'PhotometricInterpretation'):
                    if ds.PhotometricInterpretation == "YBR_FULL":
                        for i in range(num_frames):
                            ds.pixel_array[i] = convert_color_space(ds.pixel_array[i], "YBR_FULL", "RGB")
                            frameArr.append(ds.pixel_array[i])
                else:
                    for i in range(num_frames):
                        frameArr.append(ds.pixel_array[i])
            else:
                num_frames += 1
                if hasattr(ds, 'PhotometricInterpretation'):
                    if ds.PhotometricInterpretation == "YBR_FULL":
                        ds.pixel_array = convert_color_space(ds.pixel_array, "YBR_FULL", "RGB")
                frameArr.append(ds.pixel_array)

        framesNpArray = np.array(frameArr)

        return framesNpArray, study_instance_uid, series_instance_uid, image_orientation, row_spacing, column_spacing, slice_thickness, num_frames

    #return framesNpArray, study_instance_uid, series_instance_uid, image_orientation, row_spacing, column_spacing, slice_thickness, num_frames
