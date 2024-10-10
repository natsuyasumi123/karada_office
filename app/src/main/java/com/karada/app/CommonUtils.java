package com.karada.app;

import android.content.Context;
import android.util.Log;

import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.List;

public class CommonUtils {
    private static final String TAG = "CommonUtils";

    public static void copyAssetsDirToSDCard(Context context, String assetsDirName, String sdCardPath) {
        Log.d(TAG, "copyAssetsDirToSDCard() called with: context = [" + context + "], assetsDirName = [" + assetsDirName + "], sdCardPath = [" + sdCardPath + "]");
        try {
            String list[] = context.getAssets().list(assetsDirName);
            if (list.length == 0) {
                InputStream inputStream = context.getAssets().open(assetsDirName);
                byte[] mByte = new byte[1024];
                int bt = 0;
                File file = new File(sdCardPath + File.separator
                        + assetsDirName.substring(assetsDirName.lastIndexOf('/')));
                if (!file.exists()) {
                    file.createNewFile();
                } else {
                    return;
                }
                FileOutputStream fos = new FileOutputStream(file);
                while ((bt = inputStream.read(mByte)) != -1) {
                    fos.write(mByte, 0, bt);
                }
                fos.flush();
                inputStream.close();
                fos.close();
            } else {
                String subDirName = assetsDirName;
                if (assetsDirName.contains("/")) {
                    subDirName = assetsDirName.substring(assetsDirName.lastIndexOf('/') + 1);
                }
                sdCardPath = sdCardPath + File.separator + subDirName;
                File file = new File(sdCardPath);
                if (!file.exists())
                    file.mkdirs();
                for (String s : list) {
                    copyAssetsDirToSDCard(context, assetsDirName + File.separator + s, sdCardPath);
                }
            }
        } catch (
                Exception e) {
            e.printStackTrace();
        }
    }



    public static float[][] convertListToArray(List<List<NormalizedLandmark>> listOfLists) {
        if (listOfLists == null || listOfLists.isEmpty()) {
            return new float[0][0];
        }

        // 确定外部List的大小
        int rows = listOfLists.size();
        int cols = listOfLists.get(0).size()  ; // 假设每个内部List长度相同

        // 创建二维数组
        float[][] array = new float[rows][cols* 3 ];

        // 将List元素复制到二维数组
        for (int i = 0; i < rows; i++) {
            List<NormalizedLandmark> row = listOfLists.get(i);
            for (int j = 0; j < cols ; j++) {
                array[i][j * 3] = row.get(j).x() ;
                array[i][j * 3 + 1] = row.get(j).y() ;
                array[i][j * 3 + 2] = row.get(j).z() ;
            }
        }

        return array;
    }
}
