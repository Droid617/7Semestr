const start = async () => {
    const stream = await navigator.mediaDevices.getDisplayMedia({
        video:{
            mediaSource:"screen",
        },
    });

    const data = [];

    const mediaRecorder = new MediaRecorder(stream);

    mediaRecorder.ondataavailable = (e) => {
        data.push(e.data);
    }

    mediaRecorder.start();

    mediaRecorder.onstop = (e) => {
        document.querySelector("video").src = URL.createObjectURL(
            new Blob(data, {
                type: data[0].type,
            }),
        );
    };
};

start();