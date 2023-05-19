package com.wind.ndk.audio.player

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment.Companion.CenterVertically
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.lifecycle.viewmodel.compose.viewModel
import com.wind.ndk.audio.player.ui.theme.NdkaudioplayerTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            NdkaudioplayerTheme {
                AudioApp(
                    viewModel = viewModel(factory = AudioViewModel.Factory)
                )
            }
        }
    }

}


@Composable
fun AudioApp(
    viewModel: AudioViewModel
) {
    val dir = LocalContext.current.getExternalFilesDir(null)?.absolutePath
    val source = "$dir/131.mp3"
    AudioApp(
        onClickPlayAudioTrack = {
            viewModel.onClickPlayAudioTrack(source)
        },
        onClickStopAudioTrack = { viewModel.onClickStopAudioTrack() }
    )
}

@OptIn(ExperimentalMaterial3Api::class)

@Composable
fun AudioApp(
    onClickPlayAudioTrack: () -> Unit,
    onClickStopAudioTrack: () -> Unit,
) {
    Scaffold(
        topBar = { TopAppBar(title = { Text(text = "AudioPlayer") }) },
    ) { paddingValues ->
        Surface(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues),
            color = MaterialTheme.colorScheme.background
        ) {
            Row(
                horizontalArrangement = Arrangement.Center,
                verticalAlignment = CenterVertically
            ) {
                Button(onClick = { onClickPlayAudioTrack }) {
                    Text(text = "Play with AudioTrack")
                }
                Button(onClick = { onClickStopAudioTrack }) {
                    Text(text = "Stop AudioTrack")
                }
            }
        }
    }
}

@Preview
@Composable
fun PreviewApp() {
    NdkaudioplayerTheme {
        AudioApp(
            onClickPlayAudioTrack = {},
            onClickStopAudioTrack = {}
        )
    }
}