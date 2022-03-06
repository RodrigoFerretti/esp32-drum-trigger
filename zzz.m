% Parametric Equalizer by Geir K. Nilsen (2017)
clear all;

fs = 1000; % Sampling frequency [Hz]
S = 4; % Number of sections

Bf = [5 5 5 5]; % Bandwidth [Hz]
GB = [9 9 9 9]; % Bandwidth gain (level at which the bandwidth is measured) [dB]
G0 = [0 0 0 0]; % Reference gain @ DC [dB]
G = [8 10 12 14]; % Boost/cut gain [dB]
f0 = [200 250 300 350]; % Center freqency [Hz]

h = [1; zeros(1023,1)]; % ..for impulse response
b = zeros(S,3); % ..for feedforward filter coefficients
a = zeros(S,3); % ..for feedbackward filter coefficients

for s = 1:S;
    % Equation 2
    beta = tan(Bf(s)/2 * pi / (fs / 2)) * sqrt(abs((10^(GB(s)/20))^2 - (10^(G0(s)/20))^2)) / sqrt(abs(10^(G(s)/20)^2 - (10^(GB(s)/20))^2));
    % Equation 3 through 5
    b(s,:) = [(10^(G0(s)/20) + 10^(G(s)/20)*beta), -2*10^(G0(s)/20)*cos(f0(s)*pi/(fs/2)), (10^(G0(s)/20) - 10^(G(s)/20)*beta)] / (1+beta);
    % Equation 6 through 8
    a(s,:) = [1, -2*cos(f0(s)*pi/(fs/2))/(1+beta), (1-beta)/(1+beta)];

    % apply equation 1 recursively per section.
    h = filter(b(s,:), a(s,:), h);
end;

% Plot the frequency spectrum of the combined section impulse response h
H = db(abs(fft(h)));
H = H(1:length(H)/2);
f = (0:length(H)-1)/length(H)*fs/2;
plot(f,H)
axis([0 fs/2 0 20])
xlabel('Frequency [Hz]');
ylabel('Gain [dB]');
grid on