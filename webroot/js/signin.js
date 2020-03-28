$(document).ready(function () {
    $('form').submit(function(ev) {
        var formData = {
            'email': $('input[name=email]').val(),
            'password': $('input[name=password]').val()
        };

        $.ajax({
            type: 'POST',
            url: '/login',
            data: formData,
            dataType: 'json',
            encode: true
        })

        .done(function(resp) {
            console.log(resp);
        })

        .fail(function(resp) {
            console.log('Internal server error!');
            console.log(resp);
        });

        ev.preventDefault();
    });
});

