package cn.wch.blelib.ch579.ota.entry;

import androidx.annotation.NonNull;

public enum ImageType {
    A("A"),
    B("B"),
    UNKNOWN("UNKNOWN");

    private String description;
    ImageType(String s) {
        description=s;
    }

    @NonNull
    @Override
    public String toString() {
        return description;
    }
}
